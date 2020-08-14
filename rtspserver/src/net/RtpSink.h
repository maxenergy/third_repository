#ifndef _MEDIA_SINK_H_
#define _MEDIA_SINK_H_
#include <string>
#include <stdint.h>

#include "net/MediaSource.h"
#include "net/Event.h"
#include "net/UsageEnvironment.h"
#include "net/Rtp.h"


class RtpSink
{
public:
    typedef void (*SendPacketCallback)(void* arg1, void* arg2, RtpPacket* mediaPacket);

    RtpSink(UsageEnvironment* env, MediaSource* mediaSource, int payloadType);
	RtpSink(UsageEnvironment* env, MediaSource* mediaSource, int payloadType,uint8_t ext);
	
    virtual ~RtpSink();

    virtual std::string getMediaDescription(uint16_t port) = 0;
    virtual std::string getAttribute() = 0;

	virtual void push264(uint8_t* frame_h264,int size,char* ext_data,int exlen,unsigned int timestamp) = 0;

	virtual void SetPic_Size(int w,int h) = 0;
	
    void setSendFrameCallback(SendPacketCallback cb, void* arg1, void* arg2);
    uint32_t get_msscr(){return mSSRC;};
	uint16_t get_seq(){return mSeq;};
protected:
    virtual void handleFrame(AVFrame* frame) = 0;
    void sendRtpPacket(RtpPacket* packet);
    void start(int ms);
    void stop();

private:
    static void timeoutCallback(void*);

protected:
    UsageEnvironment* mEnv;
    MediaSource* mMediaSource;
    SendPacketCallback mSendPacketCallback;
    void* mArg1;
    void* mArg2;

    uint8_t mCsrcLen;
    uint8_t mExtension;
    uint8_t mPadding;
    uint8_t mVersion;
    uint8_t mPayloadType;
    uint8_t mMarker;
    uint16_t mSeq;
    uint32_t mTimestamp;
    uint32_t mSSRC;
	uint32_t mXdata_size;

private:
    TimerEvent* mTimerEvent;
    Timer::TimerId mTimerId;
};

#endif //_MEDIA_SINK_H_
