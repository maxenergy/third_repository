#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
#include "HttpServerlib.h"
#include "http-server-lib.h"



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

typedef struct __AI_Box{
	uint8_t x0;
	uint8_t y0;
	uint8_t x1;
	uint8_t y1;
	uint8_t ID_H;
	uint8_t ID_L;
}AI_Box;

typedef struct __AI_Data{
	uint8_t size;
    AI_Box * box;
	unsigned int timestamp;
}AI_Data;


std::mutex data_mutex;

FaceRecognitionApi *framework;
HttpServerLibApi *http_proxy;

UsageEnvironment* env;
RtspServer* server;
MediaSession* session;
RtpSink* rtpSink;
AI_Box FRI[32];
int box_count = 0;
unsigned long buf_index =0;
std::thread rtsp_push_loop;
unsigned char* loop_buf;
void process_detectet(FaceDetect::Msg bob);
int process_cmd(http_message_t *message);

