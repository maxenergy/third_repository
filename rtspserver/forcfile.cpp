#include <iostream>

#include "base/Logging.h"
#include "net/UsageEnvironment.h"
#include "base/ThreadPool.h"
#include "net/EventScheduler.h"
#include "net/Event.h"
#include "net/RtspServer.h"
#include "net/MediaSession.h"
#include "net/InetAddress.h"
#include "net/H264FileMediaSource.h"
#include "net/H264RtpSink.h"
#include<sys/types.h>
#include<sys/stat.h>

#include <thread>

#ifdef __cplusplus
extern "C" {
#endif
int cpp_init(void);
int cpp_push(unsigned char *h264_buf,int size);

#ifdef __cplusplus
}
#endif

UsageEnvironment* env;
RtspServer* server;
MediaSession* session;
RtpSink* rtpSink;

std::thread event_loop;

void event_loop_f()
{
	env->scheduler()->loop();
}

int cpp_init(void)
{
    Logger::setLogLevel(Logger::LogWarning);
    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);
    ThreadPool* threadPool = ThreadPool::createNew(2);
    env = UsageEnvironment::createNew(scheduler, threadPool);

    Ipv4Address ipAddr("0.0.0.0", 8554);
    server = RtspServer::createNew(env, ipAddr);
    session = MediaSession::createNew("live");
    MediaSource* mediaSource = H264FileMediaSource::createNew(env, "pipeh264");
    rtpSink = H264RtpSink::createNew(env, mediaSource);

    session->addRtpSink(MediaSession::TrackId0, rtpSink);
    server->addMeidaSession(session);
    server->start();

    std::cout<<"Play the media using the URL \""<<server->getUrl(session)<<"\""<<std::endl;

	event_loop=std::thread(event_loop_f);
	event_loop.detach();
    return 0;
}

int cpp_push(unsigned char * h264_buf,int size)
{
	rtpSink->push264(h264_buf,size,NULL,0,0);
	return 0;
}

