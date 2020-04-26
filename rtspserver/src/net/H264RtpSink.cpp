#include <stdio.h>
#include <string.h>

#include "net/H264RtpSink.h"
#include "base/Logging.h"
#include "base/New.h"

H264RtpSink* H264RtpSink::createNew(UsageEnvironment* env, MediaSource* mediaSource)
{
    if(!mediaSource)
        return NULL;
#ifndef CUSTOM_NEW
    return new H264RtpSink(env, mediaSource);
#else
    return New<H264RtpSink>::allocate(env, mediaSource);
#endif
}

H264RtpSink::H264RtpSink(UsageEnvironment* env, MediaSource* mediaSource) :
    RtpSink(env, mediaSource, RTP_PAYLOAD_TYPE_H264,1),
    mClockRate(90000),
    mFps(mediaSource->getFps())
{
	if(mFps == 0)
		mFps=32;
	
	printf("mFps %d !!\n",mFps);
    start(1000/mFps);
}

H264RtpSink::~H264RtpSink()
{

}

std::string H264RtpSink::getMediaDescription(uint16_t port)
{
    char buf[100] = {0};
    sprintf(buf, "m=video %hu RTP/AVP %d", port, mPayloadType);

    return std::string(buf);
}

std::string H264RtpSink::getAttribute()
{
    char buf[100];
    sprintf(buf, "a=rtpmap:%d H264/%d\r\n", mPayloadType, mClockRate);
    sprintf(buf+strlen(buf), "a=framerate:%d", mFps);

    return std::string(buf);
}

void H264RtpSink::push264(uint8_t* frame_h264,int size,char* ext_data,int exlen)
{
//fill extdata
		RtpHeader* rtpHeader = mRtpPacket.mRtpHeadr;
		int xsize = exlen;
		if(xsize>0){
			rtpHeader->xdata.info_type =0x11;
			rtpHeader->xdata.notuse=0x12;
			rtpHeader->xdata.length_h=0x0;
			rtpHeader->xdata.length_l=0x32;
			
			if(xsize > 0x32*4)
				xsize = 0x32*4;
			
			memcpy(rtpHeader->xdata.xbuf,ext_data,xsize);
		}else{
			rtpHeader->xdata.info_type =0xff;
			rtpHeader->xdata.notuse=0xff;
			rtpHeader->xdata.length_h=0x0;
			rtpHeader->xdata.length_l=0x32;
			memset(rtpHeader->xdata.xbuf,0x0,0x32*4);
		}
			
//fill frame
   		AVFrame *videoFrame = new AVFrame(frame_h264, size);
		videoFrame->mFrame = videoFrame->mBuffer;
		handleFrame(videoFrame);
		
}


void H264RtpSink::handleFrame(AVFrame* frame)
{
	
	RtpHeader* rtpHeader = mRtpPacket.mRtpHeadr;
	
    uint8_t naluType = frame->mFrame[0];

    if(frame->mFrameSize <= RTP_MAX_PKT_SIZE)
    {
        memcpy(rtpHeader->payload, frame->mFrame, frame->mFrameSize);
        mRtpPacket.mSize = frame->mFrameSize;
        sendRtpPacket(&mRtpPacket);
        mSeq++;

        if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8) // 如果是SPS、PPS就不需要加时间戳
            return;
    }
    else
    {
        int pktNum = frame->mFrameSize / RTP_MAX_PKT_SIZE;       // 有几个完整的包
        int remainPktSize = frame->mFrameSize % RTP_MAX_PKT_SIZE; // 剩余不完整包的大小
        int i, pos = 1;

        /* 发送完整的包 */
        for (i = 0; i < pktNum; i++)
        {
            /*
            *     FU Indicator
            *    0 1 2 3 4 5 6 7
            *   +-+-+-+-+-+-+-+-+
            *   |F|NRI|  Type   |
            *   +---------------+
            * */
            rtpHeader->payload[0] = (naluType & 0x60) | 28; //(naluType & 0x60)表示nalu的重要性，28表示为分片
            
            /*
            *      FU Header
            *    0 1 2 3 4 5 6 7
            *   +-+-+-+-+-+-+-+-+
            *   |S|E|R|  Type   |
            *   +---------------+
            * */
            rtpHeader->payload[1] = naluType & 0x1F;
            
            if (i == 0) //第一包数据
                rtpHeader->payload[1] |= 0x80; // start
            else if (remainPktSize == 0 && i == pktNum - 1) //最后一包数据
                rtpHeader->payload[1] |= 0x40; // end

            memcpy(rtpHeader->payload+2, frame->mFrame+pos, RTP_MAX_PKT_SIZE);
            mRtpPacket.mSize = RTP_MAX_PKT_SIZE+2;
            sendRtpPacket(&mRtpPacket);

            mSeq++;
            pos += RTP_MAX_PKT_SIZE;
        }

        /* 发送剩余的数据 */
        if (remainPktSize > 0)
        {
            rtpHeader->payload[0] = (naluType & 0x60) | 28;
            rtpHeader->payload[1] = naluType & 0x1F;
            rtpHeader->payload[1] |= 0x40; //end

            memcpy(rtpHeader->payload+2, frame->mFrame+pos, remainPktSize);
            mRtpPacket.mSize = remainPktSize+2;
            sendRtpPacket(&mRtpPacket);

            mSeq++;
        }
    }
    
    mTimestamp += mClockRate/mFps;
}
