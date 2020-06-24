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
		mFps=30;
	
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
//Media Attribute (a): fmtp:96 profile-level-id=420029; packetization-mode=1; sprop-parameter-sets=Z2Q
//a=fmtp:96 packetization-mode=1;profile-level-id=42C029;sprop-parameter-sets=Z0LAKdoC0EmwEQAAAwABAAADADKPGDKg,aM4XIA==
//Media Attribute (a): a=fmtp:96 profile-level-id=420029; packetization-mode=1; sprop-parameter-sets=Z2QAKKwXKgHgCJ+WEAAAcIAAFfkIQA==,aP44sA==
//a=fmtp:96 packetization-mode=1;profile-level-id=42002A;sprop-parameter-sets=Z0IAKpY1QPAET8s3AQEBAg==,aM4xsg==
//a=fmtp:96 packetization-mode=1;profile-level-id=42002A;sprop-parameter-sets=Z0IAKpY1QPAET8s3AQEBAg==,aM4xsg==
std::string H264RtpSink::getAttribute()
{
    char buf[300];
	//Media Attribute (a): recvonly
	//x-dimensions:1920,1080
//	sprintf(buf,"s=live\r\n");
	sprintf(buf,"a=transform:1,0,0;0,1,0;0,0,1\r\n");
	sprintf(buf+strlen(buf),"a=control:trackID=0\r\n");
    sprintf(buf+strlen(buf),"a=rtpmap:%d H264/%d\r\n", mPayloadType, mClockRate);
	sprintf(buf+strlen(buf),"a=fmtp:96 packetization-mode=1;profile-level-id=42002A;sprop-parameter-sets=Z0IAKpY1QPAET8s3AQEBAg==,aM4xsg==\r\n");
	sprintf(buf+strlen(buf),"a=x-dimensions:1920,1080\r\n");
	sprintf(buf+strlen(buf), "a=framerate:%d", mFps);

    return std::string(buf);
}

void H264RtpSink::push264(uint8_t* frame_h264,int size,char* ext_data,int exlen,unsigned int timestamp)
{
//fill extdata
		Ext_RtpHeader* mExtRtpHeadr = mRtpPacket.mExtRtpHeadr;
		int xsize = exlen;
		if(xsize>0){
			mExtRtpHeadr->xdata.info_type =0x11;
			mExtRtpHeadr->xdata.notuse=0x12;
			mExtRtpHeadr->xdata.length_h=0x0;
			mExtRtpHeadr->xdata.length_l=0x32;			
			if(xsize > 0x32*4)
				xsize = 0x32*4;
			
			memcpy(mExtRtpHeadr->xdata.xbuf,ext_data,xsize);
			
			mExtension = 1;
			mXdata_size = 204;
		}else{
		    mExtension = 0;
			mXdata_size = 0;
		}
//fill frame
   		AVFrame *videoFrame = new AVFrame(frame_h264, size);
		videoFrame->mFrame = videoFrame->mBuffer;
		handleFrame(videoFrame,mExtension);
		delete videoFrame;
}

#define WITH_MARK_BIT

void H264RtpSink::handleFrame(AVFrame* frame,uint8_t flag_ex)
{
	RtpHeader* rtpHeader = mRtpPacket.mRtpHeadr;
	Ext_RtpHeader* mExtRtpHeadr = mRtpPacket.mExtRtpHeadr;
	#ifdef WITH_MARK_BIT
	mMarker = 0;
    uint8_t naluType = frame->mFrame[4];
	#else
	uint8_t naluType = frame->mFrame[0];
	#endif

    if(frame->mFrameSize <= RTP_MAX_PKT_SIZE)
    {
    	if(flag_ex){
        	memcpy(mExtRtpHeadr->payload, frame->mFrame+4, frame->mFrameSize);
    	}else{
			memcpy(rtpHeader->payload, frame->mFrame+4, frame->mFrameSize);
		}
		if((naluType == 0x65)||(naluType == 0x61))
		{
		
			#ifdef WITH_MARK_BIT
			mMarker = 1;
			#endif
		}
        mRtpPacket.mSize = frame->mFrameSize-4;
        sendRtpPacket(&mRtpPacket);
        mSeq++;
		mExtension = 0;
		mXdata_size = 0;
        if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8) // 如果是SPS、PPS就不需要加时间戳
            return;
    }
    else
    {
        int pktNum = (frame->mFrameSize-5) / RTP_MAX_PKT_SIZE;       // 有几个完整的包
        int remainPktSize = (frame->mFrameSize-5) % RTP_MAX_PKT_SIZE; // 剩余不完整包的大小
        int i = 0;
		int pos = 0;

		//send first pkt,add xdata
		if(flag_ex){
			mExtRtpHeadr->payload[0] = (naluType & 0x60) | 28; //(naluType & 0x60)表示nalu的重要性，28表示为分片
			
			/*
			*	   FU Header
			*	 0 1 2 3 4 5 6 7
			*	+-+-+-+-+-+-+-+-+
			*	|S|E|R|  Type	|
			*	+---------------+
			* */
			mExtRtpHeadr->payload[1] = naluType & 0x1F;
			
			if (i == 0) //第一包数�
			{
				mExtRtpHeadr->payload[1] |= 0x80; // start
				pos = pos+5;
			}
			else if (remainPktSize == 0 && i == pktNum - 1) //最后一包数据
			{
				mExtRtpHeadr->payload[1] |= 0x40; // end
				
				#ifdef WITH_MARK_BIT
				mMarker = 1;
				#endif
			}else{
				#ifdef WITH_MARK_BIT
				mMarker = 0;
				#endif
			}
			memcpy(mExtRtpHeadr->payload+2, frame->mFrame+pos, RTP_MAX_PKT_SIZE);
			mRtpPacket.mSize = RTP_MAX_PKT_SIZE+2;
			sendRtpPacket(&mRtpPacket);
			mSeq++;
			pos += RTP_MAX_PKT_SIZE;			
			i ++;
			mExtension = 0;
			mXdata_size = 0;
		}
			

        /* 发送完整的包 */
        for (; i < pktNum; i++)
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
            {
                rtpHeader->payload[1] |= 0x80; // start
                pos = pos+5;
            }
            else if (remainPktSize == 0 && i == pktNum - 1) //最后一包数据
            {
				rtpHeader->payload[1] |= 0x40; // end
				#ifdef WITH_MARK_BIT
				mMarker = 1;
				#endif
            }else{
				#ifdef WITH_MARK_BIT
				mMarker = 0;
				#endif
			}

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
			#ifdef WITH_MARK_BIT
				mMarker = 1;
			#endif
            sendRtpPacket(&mRtpPacket);

            mSeq++;
        }
    }
    
    mTimestamp += mClockRate/mFps;
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
