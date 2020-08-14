#ifndef _H264_MEDIA_SINK_H_
#define _H264_MEDIA_SINK_H_
#include <stdint.h>

#include "net/RtpSink.h"

class H264RtpSink : public RtpSink
{
public:
    static H264RtpSink* createNew(UsageEnvironment* env, MediaSource* mediaSource);
    
    H264RtpSink(UsageEnvironment* env, MediaSource* mediaSource);
	virtual void push264(uint8_t* frame_h264,int size,char* ext_data,int exlen,unsigned int timestamp);
	virtual void SetPic_Size(int w,int h);
    virtual ~H264RtpSink();

    virtual std::string getMediaDescription(uint16_t port);
    virtual std::string getAttribute();
    virtual void handleFrame(AVFrame* frame);
	void handleFrame(AVFrame* frame,uint8_t flag_ex);
private:
    RtpPacket mRtpPacket;
    int mClockRate;
    int mFps;
	int width;
	int height;

};

#endif //_H264_MEDIA_SINK_H_
