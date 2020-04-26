#include "main.h"

extern "C" {
extern int sysinit(int* fd);
extern void log_level_set(int level);
extern void init_pola_sdk(int liveless_mode,float detect_threshold);
}

int main()
{
	int fd =0;
	std::cout << "pola sdk init!!! " << std::endl;
	sysinit(&fd);
	log_level_set(1);
	init_pola_sdk(1,0.4);

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
    printf("Play the media using the URL %s  ",server->getUrl(session).c_str());
    env->scheduler()->loop();
}

void venc_h264_callback(unsigned char *ph264,int size)
{
	data_mutex.lock();
	rtpSink->push264(ph264,size,(char *)FRI,box_count*sizeof(Face_Recognition_Item));
	box_count = 0;
	memset((char*)FRI,0x0,200);
	data_mutex.unlock();
}
	
void save_box(MtcnnInterface::Out list)
{
	int i =0;
	int cpysize = 0;
	
	data_mutex.lock();
	for (MtcnnOut box : list.mOutList) {

	 if(i > 3)
	 	break;
	 
     FRI[i].x0 = (int)(box.mRect.tl().x*800.f/1080.f);
	 FRI[i].y0 = (int)(box.mRect.tl().y*1280.f/1920.f);
     FRI[i].x1 = (int)(box.mRect.br().x*800.f/1080.f);
	 FRI[i].y1 = (int)(box.mRect.br().y*1280.f/1920.f);
	 memset(FRI[i].info,0x0,24);

	 if(box.mUserID != -1){
	 	cpysize = strlen(box.mUserName.c_str());

		if(cpysize > MAX_INFO_SIZE)
			cpysize= MAX_INFO_SIZE-1;
		
	 	memcpy(FRI[i].info,box.mUserName.c_str(),cpysize);
	 }else{	
			sprintf(FRI[i].info,"unknown!");
	 	}
	 i++;
	}
	box_count = i;
	data_mutex.unlock();
}

void process_detectet(FaceDetect::Msg bob)
{
	save_box(bob.mMtcnnInterfaceOut);
	printf("process_detectret \n");
	return;
}

void process_cmd(char cmd,void* data)
{
	Face_Record_Data *userdata = (Face_Record_Data *)data;

	if(cmd == FACE_RECORD){
		UserInfo user;
		FaceRecognitionApi::getInstance().updateFaceInfo(userdata->user_id,userdata->pic_frame);
		user.mUserName = std::string(userdata->name);
		user.mUserID = userdata->user_id ;
		if (FaceRecognitionApi::getInstance().updateUserInfo(user) == false) {
			printf("face record failed! \n");
		}
	}else{
		printf("unkown cmd!!\n");
	}
}

