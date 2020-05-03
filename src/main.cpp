#include "main.h"
//#include <time.h>

#include "sample_comm.h"

extern "C" {
extern int sysinit(int* fd);
extern void log_level_set(int level);
extern void init_pola_sdk(int liveless_mode,float detect_threshold);
extern  HI_VOID SAMPLE_COMM_VENC_RegCallback(VENC_CHN vencChn, SAMPLE_VENC_CALLBACK_S *callBack);
}

#ifdef DEBUG_FILE
int fd = 0;
#endif
#if 0
std::time_t star_time_ms = 0;

std::time_t getTimeStamp(void)
{
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::time_t timestamp =  tp.time_since_epoch().count(); 
    return timestamp;
}
#endif

void venc_h264_callback(unsigned char *ph264,int size)
{
	static int count = 0;
	unsigned char *data_buf;
	unsigned char *xdata_buf;
	unsigned int timestamp;

	//get h264 buf!
	data_buf =(unsigned char *)malloc(size);
	memcpy(data_buf,ph264,size);

	//get ai data
	data_mutex.lock();
	count = box_count;
	xdata_buf =(unsigned char *)malloc(count*sizeof(AI_Box)+1);
	xdata_buf[0] = count;
	memcpy(xdata_buf+1,FRI,count*sizeof(AI_Box));
	data_mutex.unlock();
	
	if(count){
		rtpSink->push264(data_buf,size,(char *)xdata_buf,count*sizeof(AI_Box)+1,0);
	}
	else{
		rtpSink->push264(data_buf,size,(char *)xdata_buf,0,0);
	}
#ifdef DEBUG_FILE
	write(fd,data_buf,size);
#endif
	free(data_buf);
	free(xdata_buf);
}

int main()
{
	
#ifdef DEBUG_FILE
	fd=open("raw.h264",O_RDWR|O_CREAT);
#endif
	int fd_sys;
	std::cout << "pola sdk init!!! " << std::endl;
	SAMPLE_VENC_CALLBACK_S callBack;
	callBack.pfnDataCB = venc_h264_callback;
	sysinit(&fd_sys);
	log_level_set(1);
	init_pola_sdk(1,0.4);

    http_proxy = new HttpServerLibApi();
	
    http_proxy->StartHttpServer();

    http_proxy->SetHttpServerCallback(process_cmd);

	FaceRecognitionApi &framework = FaceRecognitionApi::getInstance();
	 if (!framework.init()) {
		 printf("framework init failed \n");
		 return 1;
	 };

	framework.startCameraPreview();
	framework.setCameraPreviewCallBack([&](FaceDetect::Msg bob){
	process_detectet(bob);
	});

    
    Logger::setLogLevel(Logger::LogWarning);

    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);
    ThreadPool* threadPool = ThreadPool::createNew(2);
    env = UsageEnvironment::createNew(scheduler, threadPool);

    Ipv4Address ipAddr("0.0.0.0", 8554);
    server = RtspServer::createNew(env, ipAddr);
    session = MediaSession::createNew("live");
    MediaSource* mediaSource = H264FileMediaSource::createNew(env, "pipe264");
    rtpSink = H264RtpSink::createNew(env, mediaSource);
    session->addRtpSink(MediaSession::TrackId0, rtpSink);
    server->addMeidaSession(session);
    server->start();
	SAMPLE_COMM_VENC_RegCallback(SAMPLE_VENC_CHNID, &callBack);
    printf("Play the media using the URL %s  ",server->getUrl(session).c_str());
    env->scheduler()->loop();
#ifdef DEBUG_FILE
	close(fd);
#endif
}

void save_box(MtcnnInterface::Out list)
{
	int i =0;
	int cpysize = 0;
	data_mutex.lock();
	for (MtcnnOut box : list.mOutList) {
	 if(i > 31)
	 	break;
     FRI[i].x0 = (unsigned char)(box.mRect.tl().x*255.f/1920.f);
	 FRI[i].y0 = (unsigned char)(box.mRect.tl().y*255.f/1080.f);
     FRI[i].x1 = (unsigned char)(box.mRect.br().x*255.f/1920.f);
	 FRI[i].y1 = (unsigned char)(box.mRect.br().y*255.f/1080.f);
	 FRI[i].ID_L= box.mUserID&0xff;
     FRI[i].ID_H = box.mUserID&0xffff>>8;
	 i++;
	}
	box_count = i;
	data_mutex.unlock();
}

void process_detectet(FaceDetect::Msg bob)
{
	save_box(bob.mMtcnnInterfaceOut);
	return;
}

int process_cmd(http_message_t *message)
{
	if(message->nCmd == CMD_IAMGE){
		image_info_t *userdata = (image_info_t *)message->nMessage;
		UserInfo user;
		user.mUserName = userdata->Name;
		user.mUserID = atoi(userdata->ID);
		printf("name %s ID %d \n",user.mUserName.c_str(),user.mUserID);
		//FaceRecognitionApi::getInstance().updateFaceInfo(userdata->user_id,userdata->pic_frame);
		//if (FaceRecognitionApi::getInstance().updateUserInfo(user) == false) {
		//  printf("face record failed! \n");
		//}
	}else{
		printf("unkown cmd!!\n");
	}
		return 0;
}

