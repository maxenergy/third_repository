#include <unistd.h>
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

#include "facerecognition_public.h"
#include "facedetect.h"
#include <mutex>

#define MAX_INFO_SIZE 24
#define FACE_RECORD 0x11

typedef struct _Face_Recognition_Item{
	int x0;
	int y0;
	int x1;
	int y1;
	char info[MAX_INFO_SIZE];
} Face_Recognition_Item;

typedef struct _Face_Record_Data{
	int user_id;
	char name[24];
	VIFrame pic_frame;
}Face_Record_Data;


std::mutex data_mutex;

FaceRecognitionApi *framework;

UsageEnvironment* env;
RtspServer* server;
MediaSession* session;
RtpSink* rtpSink;
Face_Recognition_Item FRI[5];
int box_count = 0;
void process_detectet(FaceDetect::Msg bob);
