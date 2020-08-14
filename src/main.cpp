#include <iostream>
#include <string>
using namespace std;

#include "main.h"
#include "sample_comm.h"
#include "mpi_awb.h"
#include "minIni.h"
extern "C" {
extern int sysinit(int *fd_out,int mroute,int mflip);
extern void log_level_set(int level);
extern void init_pola_sdk(int liveless_mode,float detect_threshold,int test_flag);
extern  HI_VOID SAMPLE_COMM_VENC_RegCallback(VENC_CHN vencChn, SAMPLE_VENC_CALLBACK_S *callBack);
#ifdef HAS_SIP_FEATURE
extern int hisi_audio_open();
#endif
}
extern unsigned int creat_venc_osd(int channel_id,int x,int y,int w,int h,unsigned int handle);
extern void update_osd(unsigned int handle,char* bitmap_buf, int w,int h,char * ip_title);
#define GPIO_IR_LED_IN      2
#define GPIO_MIR_S0 		4
#define GPIO_MIR_S1 		5
#define GPIO_EB_ALARM       84

#ifdef USEFOR_ENTRANCE_GUARD
#define GPIO_DOOR_PORT       84
#endif


#ifdef HAS_SIP_FEATURE
#define GPIO_SIP_BUTN       6
#define GPIO_SPK_PA       83
#endif

#define DELAY_TIME_FOR_COUNT  3

//#define DEBUG_FILE

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

//trigger wdt
	wdt_alive_flag = 1;
	//get h264 buf!
	data_buf =(unsigned char *)malloc(size);
	memcpy(data_buf,ph264,size);
	//printf("data_buf:%x %x %x %x %x \n",data_buf[0],data_buf[1],data_buf[2],data_buf[3],data_buf[4]);

	//get ai data
	data_mutex.lock();
	count = box_count+box_count_obj;
	xdata_buf =(unsigned char *)malloc(count*sizeof(AI_Box)+1);
	xdata_buf[0] = count;
	memcpy(xdata_buf+1,FRI,box_count*sizeof(AI_Box));
	memcpy(xdata_buf+1+box_count*sizeof(AI_Box),XDI,box_count_obj*sizeof(AI_Box));
	data_mutex.unlock();

	if((count)||(clean_flag)){
		rtpSink->push264(data_buf,size,(char *)xdata_buf,count*sizeof(AI_Box)+1,0);
	}
	else{
		rtpSink->push264(data_buf,size,(char *)xdata_buf,0,0);
	}

	if(count)
		clean_flag = 1;
	else
		clean_flag = 0;
#ifdef DEBUG_FILE
	write(fd,data_buf,size);
#endif
	free(data_buf);
	free(xdata_buf);
}


void venc_h264_callback_main(unsigned char *ph264,int size)
{
	unsigned char *data_buf;	
	wdt_alive_flag = 1;

	data_buf =(unsigned char *)malloc(size);
	memcpy(data_buf,ph264,size);
	rtpSink_main->push264(data_buf,size,(char *)NULL,0,0);
	free(data_buf);
}



void sync_time()
{
	system("ntpclient -s -d -c 1 -i 5 -h pool.ntp.org");
}

#ifdef HAS_SIP_FEATURE
/* Callback called by the library upon receiving incoming call */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
			     pjsip_rx_data *rdata)
{
    pjsua_call_info ci;
    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);
    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(1,("main.cpp", "Incoming call from %.*s!!",
			 (int)ci.remote_info.slen,
			 ci.remote_info.ptr));
    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer(call_id, 200, NULL, NULL);
	sip_call_state = ci.state;
}

/* Callback called by the library when call's state has changed */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;
    PJ_UNUSED_ARG(e);
    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(1,("main.cpp", "Call %d state=%.*s", call_id,
			 (int)ci.state_text.slen,
			 ci.state_text.ptr));
	sip_call_state = ci.state;
}


static void arrange_window(pjsua_vid_win_id wid)
{
    pjmedia_coord pos;
    int i, last;

    pos.x = 0;
    pos.y = 10;
    last = (wid == PJSUA_INVALID_ID) ? PJSUA_MAX_VID_WINS : wid;

    for (i=0; i<last; ++i) {
	pjsua_vid_win_info wi;
	pj_status_t status;

	status = pjsua_vid_win_get_info(i, &wi);
	if (status != PJ_SUCCESS)
	    continue;

	if (wid == PJSUA_INVALID_ID)
	    pjsua_vid_win_set_pos(i, &pos);

	if (wi.show)
	    pos.y += wi.size.h;
    }

    if (wid != PJSUA_INVALID_ID)
	pjsua_vid_win_set_pos(wid, &pos);
}


/* General processing for media state. "mi" is the media index */
static void on_call_generic_media_state(pjsua_call_info *ci, unsigned mi,
                                        pj_bool_t *has_error)
{
    const char *status_name[] = {
        "None",
        "Active",
        "Local hold",
        "Remote hold",
        "Error"
    };

    PJ_UNUSED_ARG(has_error);

    pj_assert(ci->media[mi].status <= PJ_ARRAY_SIZE(status_name));
    pj_assert(PJSUA_CALL_MEDIA_ERROR == 4);
}

/* Process video media state. "mi" is the media index. */
static void on_call_video_state(pjsua_call_info *ci, unsigned mi,
                                pj_bool_t *has_error)
{
    if (ci->media_status != PJSUA_CALL_MEDIA_ACTIVE)
	return;

    arrange_window(ci->media[mi].stream.vid.win_in);

    PJ_UNUSED_ARG(has_error);
}


/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info ci;
    unsigned mi;
    pj_bool_t has_error = PJ_FALSE;
    pjsua_call_get_info(call_id, &ci);
/*
    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
	// When media is active, connect call to sound device.
	pjsua_conf_connect(ci.conf_slot, 0);
	pjsua_conf_connect(0, ci.conf_slot);
    }
*/
	//arrange_window
     
    pjsua_call_get_info(call_id, &ci);

    for (mi=0; mi<ci.media_cnt; ++mi) {
	on_call_generic_media_state(&ci, mi, &has_error);

		switch (ci.media[mi].type) {
		case PJMEDIA_TYPE_AUDIO:
		    	pjsua_conf_connect(ci.conf_slot, 0);
				pjsua_conf_connect(0, ci.conf_slot);
		    break;
		case PJMEDIA_TYPE_VIDEO:
		 //   on_call_video_state(&ci, mi, &has_error);
		    break;
		default:
		    /* Make gcc happy about enum not handled by switch/case */
		    break;
		}
    }
	
	if (has_error) {
	pj_str_t reason = pj_str("Media failed");
	pjsua_call_hangup(call_id, 500, &reason, NULL);
	}
}
void sip_setup()
{
	pjsua_callback cb;
	system("echo 6 > /sys/class/gpio/export");
	cb.on_incoming_call = &on_incoming_call;
    cb.on_call_media_state = &on_call_media_state;
	cb.on_call_state = &on_call_state;
	sip_client_init(&cb,5060);
	sip_client_register(&sip_acc_id,"1001","121.40.80.20","12345");
	hisi_audio_open();
}
#endif


#ifdef BUILD_SYSTEM_DEMO
int ota_sw_ver = 1;
int ota_hw_ver = 1;
void get_version(uint8_t *sw,uint8_t* hw)
{
	int fd = -1;
	char data[10];
	int ret =0;
	if(access("sw_version", F_OK) == 0){
	   fd = open("sw_version", O_RDONLY);
		if (fd < 0)
		{
			perror("file open sw_version!\n ");    
			return;
		}

		ret = read(fd, data, 10);

		for(int i =0;i<ret;i++)
		{
			char a = data[i] - 0x30;
			if(a<0||a>9)
			{
				data[i] = 0;
			}
		}
		*sw = atoi(data);
	}

	if(access("hw_version", F_OK) == 0){
	   fd = open("hw_version", O_RDONLY);
		if (fd < 0)
		{
			perror("file open sw_version!\n ");    
			return;
		}

		ret = read(fd, data, 10);

		for(int i =0;i<ret;i++)
		{
			char a = data[i] - 0x30;
			if(a<0||a>9)
			{
				data[i] = 0;
			}
		}
		*hw = atoi(data);
	}
}
int main()
{
	sw_ver = -1;
	hw_ver = -1;
	printf("ota sw_ver %d ota hw_ver %d \n",ota_sw_ver,ota_hw_ver);
	setup_env();
	eth0_ifconfig();
	vendor_checkota();
	
	get_version(&sw_ver,&hw_ver);
	
	printf("cam demo sw_ver is %d hw_ver is %d \n",sw_ver,hw_ver);
	Aiot_server = new cloud_server(mqtt_uptopic,mqtt_downtopic,ota_mqtt_downtopic,sw_ver,hw_ver);
	ota_setup_once=std::thread(&thread_ota_setup);
	ota_setup_once.detach();
	
	log_manager_loop = std::thread(&thread_log_manager);
	log_manager_loop.detach();
	while(1)
	{
		sleep(10);
	}
}
#else
int main()
{
	int ret =0;
#ifdef DEBUG_FILE
	fd=open("raw.h264",O_RDWR|O_CREAT);
#endif
	char save_swver_buf[20];
	char save_hwver_buf[20];

	memset(save_swver_buf,0,20);
	memset(save_hwver_buf,0,20);
	
	printf("sw_ver %d hw_ver %d \n",sw_ver,hw_ver);
	
	sprintf(save_swver_buf,"echo %d > sw_version",sw_ver);
	sprintf(save_hwver_buf,"echo %d > hw_version",hw_ver);

	system(save_swver_buf);
	system(save_hwver_buf);

	setup_env();
	eth0_ifconfig();

	wdt_loop = std::thread(&thread_wdt_loop);
	wdt_loop.detach();
	checkota();
	
	sync_time();

	tcp_cmd = new Tcp_Cmd(TCP_CMD_PORT,update_face,tcp_ota_func);
	int fd_sys;
	std::cout << "pola sdk init!!! " << std::endl;
	SAMPLE_VENC_CALLBACK_S callBack,callBack_main;
	callBack.pfnDataCB = venc_h264_callback;
	callBack_main.pfnDataCB = venc_h264_callback_main;
	sysinit(&fd_sys,camroute,camflip);
	creat_venc_osd(0,16,16,960,160,handle_main);
	creat_venc_osd(2,16,16,960,160,handle_sub);
#ifndef BUILD_FACTORY_TEST_APP
	log_level_set(1);
	init_pola_sdk(0,0.4,test_flag);
#endif
	prs485 = new rs485_port(0,9600);
	clean_flag = 0;

	Aiot_server = new cloud_server(mqtt_uptopic,mqtt_downtopic,ota_mqtt_downtopic,sw_ver,hw_ver);	
	Aiot_server->set_cmd_cb([&](down_event event){
		return process_aiotevent(event);
	});
	save_file_loop=std::thread(&thread_save_file);
	save_file_loop.detach();

	aiot_setup_once = std::thread(&thread_aiot_setup);
	aiot_setup_once.detach();

	ir_cut_thread = std::thread(&thread_ircut);
	ir_cut_thread.detach();
	
	FaceRecognitionApi &framework = FaceRecognitionApi::getInstance();
	if (!framework.init()) {
		printf("framework init failed \n");
		return 1;
	};
#if 1
	framework.startCameraPreview();
	framework.setCameraPreviewCallBack([&](FaceDetect::Msg bob){
	process_detectet(bob);
	});

#ifdef ZQ_DETECT_E-BICYCLE
	framework.setXdetectCallBack([&](FaceDetect::Msg bob){
	process_xdetectet(bob);
	});
#endif
	qrcode_loop = std::thread(&thread_qrcode_setup);
	qrcode_loop.detach();

	log_manager_loop = std::thread(&thread_log_manager);
	log_manager_loop.detach();

	system_init_stoped = 1;

#ifdef HAS_SIP_FEATURE
	sip_event_loop = std::thread(&sip_process_loop);
	sip_event_loop.detach();
#endif

#ifdef USEFOR_ENTRANCE_GUARD
	door_event_loop = std::thread(&door_process_loop);
	door_event_loop.detach();
#endif
	

    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);
 
    ThreadPool* threadPool = ThreadPool::createNew(2);	
    env = UsageEnvironment::createNew(scheduler, threadPool);
     Ipv4Address ipAddr("0.0.0.0", 554);
      server = RtspServer::createNew(env, ipAddr);
    session = MediaSession::createNew("live");	
    session_main = MediaSession::createNew("main");
    MediaSource* mediaSource = H264FileMediaSource::createNew(env, "live");
    MediaSource* mediaSource_main = H264FileMediaSource::createNew(env, "main");

	rtpSink = H264RtpSink::createNew(env, mediaSource);
	rtpSink_main = H264RtpSink::createNew(env, mediaSource_main);
	rtpSink_main->SetPic_Size(1280,720);
    session->addRtpSink(MediaSession::TrackId0, rtpSink);
	session_main->addRtpSink(MediaSession::TrackId0, rtpSink_main);

    server->addMeidaSession(session);
	server->addMeidaSession(session_main);
	
    server->start();
	
	SAMPLE_COMM_VENC_RegCallback(0, &callBack_main);
	SAMPLE_COMM_VENC_RegCallback(2, &callBack);
    printf("Play the media using the URL %s  ",server->getUrl(session).c_str());
    env->scheduler()->loop();
#endif
#ifdef DEBUG_FILE
		close(fd);
#endif
}
#endif

void thread_qrcode_setup()
{
	bool scan_ret = false;
	printf("qrcode thread started!\n");
	while(1)
	{
		usleep(1000 * 500);
		if(strlen(camera_ip_addr) <= 7){
			update_osd(handle_main,bitmap_buf_main, 960, 160,camera_ip_addr_raw);
			update_osd(handle_sub,bitmap_buf_sub, 960, 160,camera_ip_addr_raw);
		}else{
			update_osd(handle_main,bitmap_buf_main, 960, 160,camera_ip_addr);
			update_osd(handle_sub,bitmap_buf_sub, 960, 160,camera_ip_addr);
		}
		VIFrame mBtnPhoto;
		FaceRecognitionApi::getInstance().capture(QRCODE_PIC_CHN,mBtnPhoto);
		cv::Mat yuvFrame = cv::Mat(mBtnPhoto.mHeiht*3/2, mBtnPhoto.mWidth, CV_8UC1, mBtnPhoto.mData);
		cv::Mat rgbImage;
		cv::Mat grayImage;
		cv::Mat flipImage;
		cv::cvtColor(yuvFrame, rgbImage, cv::COLOR_YUV420sp2RGB);
		cv::cvtColor(rgbImage,grayImage , cv::COLOR_RGB2GRAY);

		if(camflip) /* camflip: 1-flip; */
			cv::flip(grayImage, flipImage, 1);
		else
			flipImage = grayImage;
		scan_ret = scan_image(flipImage);
		mBtnPhoto.release();
	}
}

void save_box(MtcnnInterface::Out list)
{
	int i =0;
	int cpysize = 0;
	data_mutex.lock();
	for (MtcnnOut box : list.mOutList) {
	 if(i > 15)
	 	break;
     FRI[i].x0 = (unsigned char)(box.mRect.tl().x*255.f/1920.f);
	 FRI[i].y0 = (unsigned char)(box.mRect.tl().y*255.f/1080.f);
     FRI[i].x1 = (unsigned char)(box.mRect.br().x*255.f/1920.f);
	 FRI[i].y1 = (unsigned char)(box.mRect.br().y*255.f/1080.f);
	 FRI[i].userid= htonl(box.mUserID);
	 i++;
	}
	box_count = i;
	data_mutex.unlock();
}

#ifdef USEFOR_ENTRANCE_GUARD
void open_door()
{	
	door_mutex.lock();
	Door_state_count = 1;
	door_mutex.unlock();
}

void door_setup(){
	system("echo 84 > /sys/class/gpio/export");
	system("echo \"out\" > /sys/class/gpio/gpio84/direction");
	system("echo 0 > /sys/class/gpio/gpio84/value");
}

void door_process_loop()
{
	int but_state = 0;
	int event = 0;
	door_setup();
	while(1){
		usleep(500*1000);
		door_mutex.lock();
		if(Door_state_count > 0){
			gpio_write(GPIO_DOOR_PORT,1);
			printf("open door!");
			Door_state_count++;
			if(Door_state_count >= 10){
				gpio_write(GPIO_DOOR_PORT,0);
				printf("close door!");
				Door_state_count = 0;
			}
		}
		door_mutex.unlock();
	}
}
#endif

void eb_alarm(int status)
{
	gpio_write(GPIO_EB_ALARM,status);
}

void eliminate_dithering(int status)
{
	static int last_status =0;
	static int x_count = 0;
	if(last_status == status){
		x_count++;
	}else {
		x_count = 0;
		printf("filter dithering!\n");
	}
	last_status = status;
	
	if(x_count >= DELAY_TIME_FOR_COUNT){
	  if(last_status == 1){
	    eb_alarm(1);
	  }else{
	  	eb_alarm(0);
	  }
	}
}

void save_box_obj(ObjectDetectInterface::Out list)
{
	int i =0;
	data_mutex.lock();
	for (ObjectDetectOut box : list.mOutList) {
	 if(i > 5)
	 	break;
     XDI[i].x0 = (unsigned char)(box.mBox[0]*255.f/1920.f);
	 XDI[i].y0 = (unsigned char)(box.mBox[2]*255.f/1080.f);
     XDI[i].x1 = (unsigned char)(box.mBox[1]*255.f/1920.f);
	 XDI[i].y1 = (unsigned char)(box.mBox[3]*255.f/1080.f);
	 XDI[i].userid= htonl(box.obj_id);
	 i++;
	}
	box_count_obj = i;
	if(box_count_obj > 0)
		eliminate_dithering(1);
	else
		eliminate_dithering(0);
	data_mutex.unlock();
}


void get_msg_info(up_event *item)
{
    struct tm *t;
	time_t tt;
	time(&tt);
	t = localtime(&tt);
	
	current_sec = t->tm_sec;
	current_min = t->tm_min;
	item->year = t->tm_year + 1900;
	item->month = t->tm_mon;
	item->date = t->tm_mday;
	item->hour = t->tm_hour;
	item->min = t->tm_min;
	item->sec = t->tm_sec;
	sprintf(item->file_name,"%s%4d%02d%02d%02d%02d%02d.jpg",mqtt_user,t->tm_year + 1900,t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

void save_pic(char* name)
{
	VIFrame mBtnPhoto;
	char full_name[255];
	FaceRecognitionApi::getInstance().capture(OTA_PIC_CHN,mBtnPhoto);
//	struct timeval start_time;
//	struct timeval stop_time;

	cv::Mat yuvFrame = cv::Mat(mBtnPhoto.mHeiht*3/2, mBtnPhoto.mWidth, CV_8UC1, mBtnPhoto.mData);
	cv::Mat dstImage;
//	 gettimeofday(&start_time, NULL);
	 cv::cvtColor(yuvFrame, dstImage, cv::COLOR_YUV420sp2BGR);
	// gettimeofday(&stop_time, NULL);
	// printf("cost %ld ms \n",(stop_time.tv_sec-start_time.tv_sec)*1000+(stop_time.tv_usec-start_time.tv_usec)/1000);

	sprintf(full_name,"%s/%s",FACE_UPPATH,name);
	cv::imwrite(full_name, dstImage);
	mBtnPhoto.release();
}

void print_timeinfo()
{
    struct tm *t;
	time_t tt;
	time(&tt);
	t = localtime(&tt);
	#ifdef BUILD_SYSTEM_DEMO
	printf("systime is  %4d-%02d-%02d %02d:%02d:%02d \n",t->tm_year + 1900,t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	#else
	printf("systime is  %4d-%02d-%02d %02d:%02d:%02d \n",t->tm_year + 1900,t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	#endif
}
	
#define MQTT_PING_TIME_PRESEC  90

void thread_aiot_setup()
{
	static int disconnect_count = 0;
	int ret =0;
	ret = Aiot_server->mqttc_init(mqtt_server_ip,mqtt_user, mqtt_passwd, mqtt_server_port, mqtt_user);
	if(ret != MQTTC_CONNECTED){
		printf("Mqttc connect failed!\n");
	}
	Aiot_server->ftp_init(ftp_serverip, ftp_user, ftp_passwd, ftp_server_port);
	while(1){
		sleep(MQTT_PING_TIME_PRESEC);
		if(Aiot_server->mqttc->HasConnected())
		{
			Aiot_server->mqttc->sendping();
		}else{			
			printf("zq mattc is disconnect,try to reconnect!\n");
			disconnect_count++;
			if(disconnect_count > 50)
			{
				system("reboot");
			}
			Aiot_server->mqttc_reconnect();
		}
		print_timeinfo();
	}
}

#define LOG_TIME_PRESEC 120

int FileSize(const char* fname)
{
    struct stat statbuf;
    if(stat(fname,&statbuf)==0)
        return statbuf.st_size;
    return -1;
}

void thread_log_manager()
{
	int logsize = 0;
	while(1){
		print_timeinfo();
		logsize = FileSize("system.log");
		printf("logsize %d \n",logsize);
		if(logsize > 4000000)
		{
			//Aiot_server->zq_updatelog();
			sleep(20);
			system("echo start > system.log");
		}
		sleep(LOG_TIME_PRESEC);
	}
}

#ifdef HAS_SIP_FEATURE
void sip_event()
{
	static int button_state = SIP_BUTN_NO_CALL;
	pj_status_t status;
	if(button_state == SIP_BUTN_NO_CALL)
	{
		if((sip_call_state == PJSIP_INV_STATE_NULL)||(sip_call_state == PJSIP_INV_STATE_DISCONNECTED))
		{
			pj_str_t uri = pj_str("sip:1000@121.40.80.20;transport=tcp");
			status = pjsua_call_make_call(sip_acc_id, &uri, 0, NULL, NULL, NULL);
			if (status != PJ_SUCCESS){
				printf("make sip call failed!\n");
			}else{
				printf("make call!!!\n");
				button_state = SIP_BUTN_HAS_CALL;
			}
		}else{
			button_state = SIP_BUTN_HAS_CALL;
		}
	}else{
		if((sip_call_state == PJSIP_INV_STATE_CONFIRMED)||(sip_call_state == PJSIP_INV_STATE_CALLING))
		{
		  pjsua_call_hangup_all();
		  button_state = SIP_BUTN_NO_CALL;
		  printf("hangup !!!\n");
		}else{
		  button_state = SIP_BUTN_NO_CALL;
		}
	}
}

void sip_process_loop()
{
	int but_state = 0;
	int event = 0;
	static int last_state = 1;
	sip_setup();
	while(1){
		usleep(100*1000);
		but_state = gpio_read(GPIO_SIP_BUTN);
		if(last_state != but_state)
		{
			if(but_state == 1)
			{
				sip_event();
			}
		}
		last_state = but_state;
	}
}
#endif

void thread_ota_setup()
{
	int ret =0;
	int ota_cnst =0;

	ret = Aiot_server->ota_mqttc_init(ota_mqtt_server_ip,ota_mqtt_user, ota_mqtt_passwd, ota_mqtt_server_port, fixed_dev_sn);
	if(ret != MQTTC_CONNECTED){
		printf("OTA Mqttc connect failed!\n");
	}
	Aiot_server->ota_ftp_init(ota_ftp_serverip, ota_ftp_user, ota_ftp_passwd, ota_ftp_server_port);
	Aiot_server->ftp_init(ftp_serverip, ftp_user, ftp_passwd, ftp_server_port);
	while(1){
		sleep(MQTT_PING_TIME_PRESEC);
		if(Aiot_server->ota_mqttc->HasConnected())
		{
			Aiot_server->ota_mqttc->sendping();
		}else{
			printf("ota mattc is disconnect,try to reconnect!\n");
			Aiot_server->ota_mqttc_reconnect();
	  	}
	}
}

int gpio_read(int gpio_id)
{
	FILE *fp = 0;
	char state = 0;
	int ret =-1;
	char file_name[255];
	sprintf(file_name,"/sys/class/gpio/gpio%d/value",gpio_id);
	fp = fopen(file_name,"r");
	if(fp > 0)
	{
		ret = fread(&state,1,1,fp);
	}
	fclose(fp);
	return (state - 0x30);
}

void gpio_write(int gpio,int on_off)
{
	FILE *fp = 0;
	char state = 0x30 + on_off;
	int ret =-1;
	char file_name[255];
	sprintf(file_name,"/sys/class/gpio/gpio%d/value",gpio);
	fp = fopen(file_name,"r+");
	if(fp > 0)
	{
		ret = fwrite(&state,1,1,fp);
	}
	fclose(fp);
}

int get_irled_state()
{
	int state = 0;
	state =  gpio_read(GPIO_IR_LED_IN);
	if(state == 0)
		return 1;
	else
		return 0;
}



void set_ircut(int on_off)
{
	if(on_off){
		 gpio_write(GPIO_MIR_S1,0);
		 gpio_write(GPIO_MIR_S0,1);
	}else{
		 gpio_write(GPIO_MIR_S1,1);
		 gpio_write(GPIO_MIR_S0,0);
	}
	usleep(1000*120);
	gpio_write(GPIO_MIR_S1,0);
    gpio_write(GPIO_MIR_S0,0);
}

void thread_ircut()
{
	int ir_st = 0;
	int ir_cut_st = 0;
	int on_count = 0;
	int off_count = 0;
	ISP_SATURATION_ATTR_S color_attr;
	ISP_SATURATION_ATTR_S gray_attr;
	HI_MPI_ISP_GetSaturationAttr(0,&color_attr);
	gray_attr.enOpType = OP_TYPE_MANUAL;
	gray_attr.stManual.u8Saturation = 0;
	system("echo 2 > /sys/class/gpio/export");
	system("echo \"in\" > /sys/class/gpio/gpio2/direction");
	system("echo 4 > /sys/class/gpio/export");
	system("echo \"out\" > /sys/class/gpio/gpio4/direction");
	system("echo 5 > /sys/class/gpio/export");
	system("echo \"out\" > /sys/class/gpio/gpio5/direction");
	system("echo 84 > /sys/class/gpio/export");
	system("echo \"out\" > /sys/class/gpio/gpio84/direction");
	system("echo 0 > /sys/class/gpio/gpio84/value");
	set_ircut(0);
	while(1)
	{
		usleep(1000*200);
		ir_st = get_irled_state();
		if(ir_st == 1){
			if(ir_cut_st == 0){
				if(++on_count > 3){
					set_ircut(1);
					ir_cut_st = 1;
					HI_MPI_ISP_SetSaturationAttr(0,&gray_attr);
				}
			}
			off_count = 0;
		}else{
			if(ir_cut_st == 1){
				if(++off_count > 3){
					set_ircut(0);
					ir_cut_st = 0;
					HI_MPI_ISP_SetSaturationAttr(0,&color_attr);
				}
			}
			on_count = 0;
		}
	}

}
void thread_save_file()
{
	while(1)
	{
		usleep(1000*50);
		int i = save_file_fifo.size();
		if(i > 0){
			Save_file *sf;
			{
		        std::unique_lock<std::mutex> locker(mFifo_Mutex);	
				sf = save_file_fifo.front();
				save_file_fifo.pop_front();
			}
			if(sf != NULL){
				save_pic(sf->file_name);
				Aiot_server->ftp_up_trigger(sf->file_name);
			}
			free(sf);
		}
	}
}


bool is_need_save_pic()
{
	if((last_sec !=current_sec)||(last_min != current_min)){
		last_sec = current_sec;
		last_min = current_min;
		return true;
	}

	return false;
}

//1.cur_floor
// 2.dst_floor
//  3. lift_direct
//  4. 

void trigger_lift(int userid)
{
	UserInfo info;
	int onoff=1;
	if(FaceRecognitionApi::getInstance().getUserInfo(userid,info) == false){
		printf("find  userinfo error!\n");
		return;
	}
//entrance guard
	prs485->ctl_sw(0,info.floor-1,1);
	prs485->ctl_sw(0,0,1);
}

void update_iot(FaceDetect::Msg &bob)
{
	up_event item;
	int count = 0;
	item.type = CMD_TYPE_FACEUP;
	for(int j = 0; j< bob.mMtcnnInterfaceOut.mOutList.size();j++)
	{
		if(bob.mMtcnnInterfaceOut.mOutList[j].mUserID > 0)
		{
			if(count < 20){
				item.userlist[count] = bob.mMtcnnInterfaceOut.mOutList[j].mUserID;
				count++;
			}
		}
	}
	if(count > 0){
		item.id_size = count;
		get_msg_info(&item);
		if(Aiot_server->Is_need_up(&item)){
			if(is_need_save_pic()){				
				std::unique_lock<std::mutex> locker(mFifo_Mutex);	
				if(save_file_fifo.size() < 10){					
					Save_file *sf =(Save_file *)malloc(sizeof(Save_file));
					memcpy(sf->file_name,item.file_name,50);
					save_file_fifo.push_back(sf);
				}else{
					printf("too many file need encode ,skip this!!!\n");
				}
			}
			Aiot_server->trigger_up(item);
			Aiot_server->mark_face(item);
		}
		
		for(int j =0;j<item.id_size;j++)
		{
			if(item.userlist[j] != -1)
			{
				trigger_lift(item.userlist[j]);
			}
		}
		
		#ifdef USEFOR_ENTRANCE_GUARD
		open_door();
		#endif
	}
}

void process_detectet(FaceDetect::Msg bob)
{
	save_box(bob.mMtcnnInterfaceOut);
	update_iot(bob);
	return;
}


#ifdef ZQ_DETECT_E-BICYCLE
void process_xdetectet(FaceDetect::Msg bob)
{
	save_box_obj(bob.mObjDetectOut);
	return;
}
#endif

/*
int process_cmd(http_message_t *message)
{
	if(message->nCmd == CMD_IAMGE){
		image_info_t *userdata = (image_info_t *)message->nMessage;
		UserInfo user;
		user.mUserName = userdata->Name;
		user.mUserID = atoi(userdata->ID);
		printf("name %s ID %d \n",user.mUserName.c_str(),user.mUserID);
	}else{
		printf("unkown cmd!!\n");
	}
		return 0;
}
*/
int update_face(Face_Upstream_First* cmd_buf)
{
	VIFrame frame;
	UserInfo user;
	user.mUserName = (char*)(cmd_buf->name);
	user.mUserID = ntohl(cmd_buf->userid);
	user.floor = 6;
	user.perm = 1;
	
	//frame.mData = (uint8_t *)(cmd_buf + sizeof(Face_Upstream_First));
	frame.mWidth = ntohl(cmd_buf->image_width);
	frame.mHeiht = ntohl(cmd_buf->image_height);
	uint8_t *data_ptr = (uint8_t *)malloc(frame.mWidth*frame.mHeiht*3);
	memcpy(data_ptr,cmd_buf + 32,frame.mWidth*frame.mHeiht*3);
	frame.mData = data_ptr;
	printf("frame.mHeiht %d frame.mWidth %d name %s %d \n",frame.mHeiht,frame.mWidth,cmd_buf->name,user.mUserID);
	frame.mFormat = VIFrame::PixelFormat::BGR;
    if(FaceRecognitionApi::getInstance().updateFaceInfo(user.mUserID,frame) == false){
			printf("update face error!\n");
			return -1;
	}
	
	if (FaceRecognitionApi::getInstance().updateUserInfo(user) == false) {
			printf("update face error!\n");
            return -1;
     }else{
			printf("update face ok!\n");
		}
	 free(data_ptr);
	return 0;
}


int aiot_add_face(char* name ,int id,int floor,char* filename)
{
	Aiot_server->ftpdown(filename);
	char name_buf[50];
	memset(name_buf,0,50);
	sprintf(name_buf,"FtpDownLoad/%s",filename);
	cv::Mat pic = cv::imread(name_buf, cv::IMREAD_UNCHANGED);
	if(pic.data){
		UserInfo user;
		VIFrame frame;
		user.mUserName = name;
		user.mUserID = id;
		user.floor = floor;
		user.perm = 1;
		
		frame.mData = (uint8_t *)(pic.data);
		frame.mWidth = pic.cols;
		frame.mHeiht = pic.rows;
		printf("frame.mHeiht %d frame.mWidth %d name %s %d \n",frame.mHeiht,frame.mWidth,user.mUserName,user.mUserID);
		frame.mFormat = VIFrame::PixelFormat::BGR;
		if(FaceRecognitionApi::getInstance().updateFaceInfo(user.mUserID,frame) == false){
				printf("update face error!\n");
				return -1;
		}
		
		if (FaceRecognitionApi::getInstance().updateUserInfo(user) == false) {
				printf("update face error!\n");
				return -1;
		 }else{
				printf("update face ok!\n");
			}
	} else{
		printf("file %s not find !\n",name_buf);
		return -1;
	}
	return 0;
}

int aiot_del_face(int userid)
{
	 if(FaceRecognitionApi::getInstance().delface(userid) == false){
			printf("update face error!\n");
			return -1;
		}
	 return 0;
}

void aiot_resetsystem()
{
	printf("aiot reset system!\n");
 	system("reboot");
}


int aiot_setperm(int userid,int perm)
{
 	UserInfo old_info;
	UserInfo new_info;
	if(FaceRecognitionApi::getInstance().getUserInfo(userid,old_info) == false){
		printf("find  userinfo error!\n");
		return -1;
	}
	new_info.mUserName = old_info.mUserName;
	new_info.floor = old_info.floor;
	new_info.mUserID = old_info.mUserID;
	new_info.perm = perm;
	if(FaceRecognitionApi::getInstance().updateUserInfo(new_info) == false){
		printf("update  userinfo error!\n");
		return -1;
	}
		return 0;
}

int process_aiotevent(down_event event)
{
	int ret =0;
	switch(event.type) {
	case CMD_TYPE_DOWN_REGFACE:
		char id_name[10];
		sprintf(id_name,"%d",event.userid);
		ret = aiot_add_face(id_name,event.userid,event.floor,event.pic_name);
		break;
	case CMD_TYPE_DOWN_DELFACE:
		ret = aiot_del_face(event.userid);
		break;
	case CMD_TYPE_DOWN_SETPERM:
		ret = aiot_setperm(event.userid,event.user_data[0]);
		break;
	case CMD_TYPE_DOWN_RESET:
		aiot_resetsystem();
		break;
	case CMD_TYPE_DOWN_SET_SECURITY_LEVEL:
		security_level = event.user_data[0];
		break;
	default:
		printf("error cmd type %d \n",event.type);
		break;
	}
	return ret;
}


int process_ota(down_event event)
{
	int ret =0;
	switch(event.type) {
	case CMD_TYPE_DOWN_REGFACE:
		char id_name[10];
		sprintf(id_name,"%d",event.userid);
		ret = aiot_add_face(id_name,event.userid,event.floor,event.pic_name);
		break;
	case CMD_TYPE_DOWN_DELFACE:
		ret = aiot_del_face(event.userid);
		break;
	case CMD_TYPE_DOWN_SETPERM:
		ret = aiot_setperm(event.userid,event.user_data[0]);
		break;
	case CMD_TYPE_DOWN_RESET:
		aiot_resetsystem();
		break;
	default:
		printf("error cmd type %d \n",event.type);
		break;
	}
	return ret;
}


void thread_wdt_loop()
{
	int ret=0;
	int sw_id = 0;
	int timeout = 10;
	int onoff=0;
 	int fd  = open(WDT_DEV_FILE, O_RDWR);
	if (fd < 0){
		fprintf(stderr, "fail to open file:%s\n", WDT_DEV_FILE);
		return;
	}
	printf("start watch dog!\n");
	ret = ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
	
	while(1)
	{
		sleep(3);
		if(wdt_alive_flag){
			wdt_alive_flag = 0;
			ret = ioctl(fd, WDIOC_KEEPALIVE);
		}else{
			if(system_init_stoped == 0){
				ret = ioctl(fd, WDIOC_KEEPALIVE);
				printf("wait for system up!\n");
			}
			else
				printf("vdec has no data!\n need reboot!\n");
		}
	}
}
int param_check(const char* str)
{
	if(sizeof(str) > MAX_PARAM_SIZE)
		return -1;
	return 0;
}

int if_param_check(char* str)
{
	if(sizeof(str) > 4)
		return -1;
	return 0;
}


void eth0_ifconfig()
{
	char ifcmd_buf[50];

	/* set eth0 ip address */
	memset(ifcmd_buf,0,50);
	sprintf(ifcmd_buf,"ifconfig eth0 %s %s", camera_ip_addr, camera_ip_mask);
	system(ifcmd_buf);

	/* set default gateway */
	memset(ifcmd_buf,0,50);
	if(strlen(camera_ip_gateway) <= 7)
	{
		char route_buf[50];
		char temp_buf[50];
		memset(temp_buf,0,50);
		memset(route_buf,0,50);
		int i = 0;
		if(strlen(camera_ip_addr) <= 7){
			memcpy(temp_buf,camera_ip_addr_raw,strlen(camera_ip_addr_raw));
			i = strlen(camera_ip_addr_raw);
		}else{
			memcpy(temp_buf,camera_ip_addr,strlen(camera_ip_addr));
			i = strlen(camera_ip_addr);
		}
		for(;i>=6;i--)
		{
			if(temp_buf[i] == '.')
			{
				break;
				printf("");
			}
		}
		memcpy(route_buf,temp_buf,i);
		sprintf(ifcmd_buf,"route add default gw %s.1", route_buf);
	}else{
		sprintf(ifcmd_buf,"route add default gw %s", camera_ip_gateway);
	}
	system(ifcmd_buf);
}

void setup_env()
{
	int index = 0, i;
	struct ifreq ifreq;
	int sock;
	uint8_t hw_addr[6];

	printf("setup env!------------\n");

	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("setup env failed! socket err!\n");
		return;
	}
	strcpy (ifreq.ifr_name, "eth0");    //Currently, only get eth0
	if (ioctl (sock, SIOCGIFHWADDR, &ifreq) < 0)
	{
		printf("setup env failed! eth0 device err!\n");
		return;
	}
	for(i=0;i<6;i++)
	{
		hw_addr[i] = ifreq.ifr_hwaddr.sa_data[i];
	}

	if (ioctl (sock, SIOCGIFADDR, &ifreq) < 0)
	{
		printf("setup env failed! eth0 device err!\n");
		return;
	}
	struct sockaddr_in sin;
	memset(camera_ip_addr_raw,0,MAX_PARAM_SIZE);
    memcpy(&sin, &ifreq.ifr_addr, sizeof(sin));
	sprintf(camera_ip_addr_raw,"%s",inet_ntoa(sin.sin_addr));
	
	memset(fixed_dev_sn,0,20);
	sprintf(fixed_dev_sn,"%s_%x%x%x%x%x%x_%d",CUSTOM_ID,hw_addr[0],hw_addr[1], \
		hw_addr[2],hw_addr[3],hw_addr[4],hw_addr[5],DEV_TYPE);

	printf("fixed_dev_sn %s \n",fixed_dev_sn);
	close(sock);

	if(access("InitConfig.ini", F_OK) == 0)
	{
		string temp;
		minIni ini("InitConfig.ini");
		
		for(i=0;i<PARAM_INDEX_HW_VER;i++)
		{
			switch(i)
			{
				case PARAM_INDEX_CAM_ROUTE:
					 camroute = ini.geti("CAMERA","camera_route",0);
					 printf("camroute is %d \n",camroute);
					 break;

				case PARAM_INDEX_CAM_FLIP:
					camflip = ini.geti("CAMERA","camera_flip",0);
					printf("camflip is %d \n",camflip);
					break;
					
				case PARAM_INDEX_TESTFLAG:
					test_flag = ini.geti("CAMERA","camera_test_flag",0);
					printf("test_flag is %d \n",test_flag);
					break;
					
				case PARAM_INDEX_MQTT_UPTOPIC:
					memset(mqtt_uptopic,0,MAX_PARAM_SIZE);
					temp = ini.gets("MQTT","mqtt_uptopic","UP/CAM/ZQ");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(mqtt_uptopic,"%s",temp.c_str());
					printf("mqtt_uptopic is %s \n",mqtt_uptopic);
					break;
				case PARAM_INDEX_MQTT_DOWNTOPIC:
					memset(mqtt_downtopic,0,MAX_PARAM_SIZE);
					temp = ini.gets("MQTT","mqtt_downtopic","CTL/CAM/ZQ");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(mqtt_downtopic,"%s",temp.c_str());
					printf("mqtt_downtopic is %s \n",mqtt_downtopic);
					break;
				case PARAM_INDEX_FTP_SERVER_IP:
					memset(ftp_serverip,0,MAX_PARAM_SIZE);
					temp = ini.gets("FTP","ftp_server_ip","222.191.244.203");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}	
					sprintf(ftp_serverip,"%s",temp.c_str());
					printf("ftp_serverip is %s \n",ftp_serverip);
					break;
				case PARAM_INDEX_FTP_SERVER_PORT:
					ftp_server_port = ini.geti("FTP","ftp_server_port",2125);
					printf("ftp_server_port is %d \n",ftp_server_port);
					break;
				case PARAM_INDEX_FTP_USERNAME:
					memset(ftp_user,0,MAX_PARAM_SIZE);
					temp = ini.gets("FTP","ftp_user","ftpuser");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ftp_user,"%s",temp.c_str());
					printf("ftp_user is %s \n",ftp_user);
					break;
				case PARAM_INDEX_FTP_PASSWD:
					memset(ftp_passwd,0,MAX_PARAM_SIZE);
					temp = ini.gets("FTP","ftp_passwd","welcome1");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ftp_passwd,"%s",temp.c_str());
					printf("ftp_passwd is %s \n",ftp_passwd);
					break;
					
				case PARAM_INDEX_MQTT_SERVER_IP:
					memset(mqtt_server_ip,0,MAX_PARAM_SIZE);
					temp = ini.gets("MQTT","mqtt_server_ip","222.191.244.203");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(mqtt_server_ip,"%s",temp.c_str());
					printf("mqtt_server_ip is %s \n",mqtt_server_ip);
					break;
					
				case PARAM_INDEX_MQTT_SERVER_PORT:
					mqtt_server_port = ini.geti("MQTT","mqtt_server_port",2884);
					printf("mqtt_server_port is %d \n",mqtt_server_port);
					break;
					
				case PARAM_INDEX_MQTT_USERNAME:
					memset(mqtt_user,0,MAX_PARAM_SIZE);
					temp = ini.gets("MQTT","mqtt_user","test");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(mqtt_user,"%s",temp.c_str());
					printf("mqtt_user is %s \n",mqtt_user);
					break;

				case PARAM_INDEX_MQTT_PASSWD:
					memset(mqtt_passwd,0,MAX_PARAM_SIZE);
					temp = ini.gets("MQTT","mqtt_passwd","123456");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(mqtt_passwd,"%s",temp.c_str());
					printf("mqtt_passwd is %s \n",mqtt_passwd);
					break;
					
				case PARAM_INDEX_OTA_MQTT_DOWNTOPIC:
					memset(ota_mqtt_downtopic,0,MAX_PARAM_SIZE);
					temp = ini.gets("OTA","ota_mqtt_downtopic","DOWN/CAM/OTA");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_mqtt_downtopic,"%s",temp.c_str());
					printf("ota_mqtt_downtopic is %s \n",ota_mqtt_downtopic);
					break;
					
				case PARAM_INDEX_OTA_MQTT_SERVER_IP:
					memset(ota_mqtt_server_ip,0,MAX_PARAM_SIZE);
					temp = ini.gets("OTA","ota_mqtt_server_ip","47.104.16.46");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_mqtt_server_ip,"%s",temp.c_str());
					printf("ota_mqtt_server_ip is %s \n",ota_mqtt_server_ip);
					break;
				case PARAM_INDEX_OTA_MQTT_SERVER_PORT:
					ota_mqtt_server_port = ini.geti("OTA","ota_mqtt_server_port",61613);
					printf("ota_mqtt_server_port is %d \n",ota_mqtt_server_port);
					break;
					
				case PARAM_INDEX_OTA_MQTT_USERNAME:
					memset(ota_mqtt_user,0,MAX_PARAM_SIZE);
					temp = ini.gets("OTA","ota_mqtt_user","admin");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_mqtt_user,"%s",temp.c_str());
					printf("ota_mqtt_user is %s \n",ota_mqtt_user);
					break;

				case PARAM_INDEX_OTA_MQTT_PASSWD:
					memset(ota_mqtt_passwd,0,MAX_PARAM_SIZE);
					temp = ini.gets("OTA","ota_mqtt_passwd","password");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_mqtt_passwd,"%s",temp.c_str());
					printf("ota_mqtt_passwd is %s \n",ota_mqtt_passwd);
					break;
					
				case PARAM_INDEX_OTA_FTP_SERVER_IP:
					memset(ota_ftp_serverip,0,MAX_PARAM_SIZE);
					temp = ini.gets("OTA","ota_ftp_server_ip","47.104.16.46");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_ftp_serverip,"%s",temp.c_str());
					printf("ota_ftp_serverip is %s \n",ota_ftp_serverip);
					break;
				
				case PARAM_INDEX_OTA_FTP_SERVER_PORT:
					ota_ftp_server_port = ini.geti("OTA","ota_ftp_server_port",21);
					printf("ota_ftp_server_port is %d \n",ota_ftp_server_port);
					break;

				case PARAM_INDEX_OTA_FTP_USERNAME:
					memset(ota_ftp_user,0,MAX_PARAM_SIZE);
					temp = ini.gets("OAT","ota_ftp_user","ftpuser1");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_ftp_user,"%s",temp.c_str());
					printf("ota_ftp_user is %s \n",ota_ftp_user);
					break;
				
				case PARAM_INDEX_OTA_FTP_PASSWD:
					memset(ota_ftp_passwd,0,MAX_PARAM_SIZE);
					temp = ini.gets("OTA","ota_ftp_passwd","Wmanhai123!");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_ftp_passwd,"%s",temp.c_str());
					printf("ota_ftp_passwd is %s \n",ota_ftp_passwd);
					break;
				
				case PARAM_INDEX_CAMERA_IP_ADDR:
					memset(camera_ip_addr,0,MAX_PARAM_SIZE);
					temp = ini.gets("CAMERA","camera_ip_addr","192.168.1.88");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(camera_ip_addr,"%s",temp.c_str());
					printf("camera_ip_addr is %s \n",camera_ip_addr);
					break;

				case PARAM_INDEX_CAMERA_IP_MASK:
					memset(camera_ip_mask,0,MAX_PARAM_SIZE);
					temp = ini.gets("CAMERA","camera_ip_mask","255.255.255.0");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(camera_ip_mask,"%s",temp.c_str());
					printf("camera_ip_mask is %s \n",camera_ip_mask);
					break;

				case PARAM_INDEX_CAMERA_IP_GATEWAY:
					memset(camera_ip_gateway,0,MAX_PARAM_SIZE);
					temp = ini.gets("CAMERA","camera_ip_gateway","192.168.1.1");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(camera_ip_gateway,"%s",temp.c_str());
					printf("camera_ip_gateway is %s \n",camera_ip_gateway);
					break;

				case PARAM_INDEX_G_DEVICE_SN:
					memset(g_device_sn,0,MAX_PARAM_SIZE);
					temp = ini.gets("DEVICE","device_sn","EMSD00010001");
					if(param_check(temp.c_str())){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(g_device_sn,"%s",temp.c_str());
					printf("g_device_sn is %s \n",g_device_sn);
					break;

#if 0
			   case PARAM_INDEX_SW_VER:
			   	    sw_ver =  atoi(temp);
					printf("sw_ver is %x \n",sw_ver);
			   	    break;
			   
			   case PARAM_INDEX_HW_VER:
			   	    hw_ver =  atoi(temp);
					printf("hw_ver is %x \n",hw_ver);
			   	    break;
#endif
				default:
					printf("error param! \n");
					break;
			}
		}

	}
}
void checkota()
{
	if(opendir(FTP_UPDATE_OTA) == NULL)
	{
		char cmd_buf[20];
		memset(cmd_buf,0,20);
		sprintf(cmd_buf,"mkdir %s",FTP_UPDATE_OTA);
		system(cmd_buf);
	}

	if(opendir(FTP_UPDATE_PATH) == NULL)
	{
		char cmd_buf[20];
		memset(cmd_buf,0,20);
		sprintf(cmd_buf,"mkdir %s",FTP_UPDATE_PATH);
		system(cmd_buf);
	}

	if(opendir(FTP_DOWN_PATH) == NULL)
	{
		char cmd_buf[20];
		memset(cmd_buf,0,20);
		sprintf(cmd_buf,"mkdir %s",FTP_DOWN_PATH);
		system(cmd_buf);
	}

	if(access("OTA/update.sh", F_OK) == 0){
		printf("wait for network!\n");
		sleep(5);//waitfor network!
		printf("start ota!\n");
		system("chmod 777 OTA/update.sh");
		system("dos2unix OTA/update.sh");
		system("OTA/update.sh");
	}
}


void vendor_checkota()
{
	if(opendir(FTP_UPDATE_OTA) == NULL)
	{
		char cmd_buf[20];
		memset(cmd_buf,0,20);
		sprintf(cmd_buf,"mkdir %s",FTP_UPDATE_OTA);
		system(cmd_buf);
	}

	if(opendir(FTP_UPDATE_PATH) == NULL)
	{
		char cmd_buf[20];
		memset(cmd_buf,0,20);
		sprintf(cmd_buf,"mkdir %s",FTP_UPDATE_PATH);
		system(cmd_buf);
	}

	if(opendir(FTP_DOWN_PATH) == NULL)
	{
		char cmd_buf[20];
		memset(cmd_buf,0,20);
		sprintf(cmd_buf,"mkdir %s",FTP_DOWN_PATH);
		system(cmd_buf);
	}

	if(access("OTA/Vupdate.sh", F_OK) == 0){
		printf("wait for network!\n");
		sleep(5);//waitfor network!
		printf("start vendor ota!\n");
		system("chmod 777 OTA/Vupdate.sh");
		system("dos2unix OTA/Vupdate.sh");
		system("OTA/Vupdate.sh");
	}
}



int  tcp_ota_func(char* cmd)
{
	printf("ota cmd is %s \n",cmd);
	system(cmd);
  return 0;
}
void HandleSig(int signum)
{
    const int len=1024;
    void *func[len];
    size_t size;
    int i;
    char **funs;
    signal(signum,SIG_DFL);
    size=backtrace(func,len);
    funs=(char**)backtrace_symbols(func,size);
    	printf("System error, Stack trace:\n");
    for(i=0;i<size;++i) 
		printf("%d %s \n",i,funs[i]);
    free(funs);
    exit(1);
}

void register_sig()
{
	signal(SIGSEGV, HandleSig);	  
	signal(SIGABRT, HandleSig);
}

int save_DevConfig(const char *config_buffer)
{
    int fd;
    int tmp_value = 0;
    size_t size;
    char QRdata[1024] = {0};
    char dev_config_data[1024] = {0};
    bool is_success;
	int has_config = 0;
    minIni ini("InitConfig.ini");

    /* judgement the security level */
    if (!security_level)
    {
        printf("WARNING: This security level %d isn't allowed to set dev_config", security_level);
		return 0;
    }

	if((config_buffer[0] == 'I')&&(config_buffer[1] == 'N')&&(config_buffer[2] == 'I')&&(config_buffer[3] == 'T')\
		&&(config_buffer[4] == 'C')&&(config_buffer[5] == 'A')&&(config_buffer[6] == 'M'))
	{
    	/* remove the header: "INITCAM:" */
    	config_buffer += 8;
	}
    memset(QRdata, 0, 1024);
    memcpy(QRdata, config_buffer, strlen(config_buffer));

    /* analysis the config information from QR code. */
    char*temp = strtok(QRdata,";");
    while(temp)
    {
    	int split_index = 0;
    	for(split_index=0;split_index<2;split_index++)
    	{
			if(temp[split_index+1] == ':')
			{
				break;
			}
		}
		if(temp[split_index+1] != ':'){
			printf("not find key %s !!!",temp);
			return -1;
		}
        char Key = temp[split_index];
        char *Value = temp+2+split_index;

        temp = strtok(NULL, ";");

        /* If Security_level is 1, only IP address and Camera Route could be set. */
        if ((security_level == 1)&&((Key!='I')||(Key!='J')||(Key!='K')||(Key=='L')))
        {
            printf("Tips: Current Security Level is %d, can't set Key %c\n", security_level, Key);
            return -1;
        }

        switch(Key)
        {
            case 'A': /* MQTT server ip address */
                memset(mqtt_server_ip,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(mqtt_server_ip,"%s",Value);

                is_success = ini.put("MQTT","mqtt_server_ip",mqtt_server_ip);
                if(!is_success)
                {
                    printf("ERROR: ini.put MQTT mqtt_server_ip failed.\n ");
                    return -1;
                }
				has_config = 1;
	        break;
            case 'B': /* MQTT server port */
                tmp_value =atoi(Value);
                if ((tmp_value<1)||(tmp_value>65535))
                {
                    printf("error: ftp port is invalid as %d.\n", tmp_value);
                    return -1;
                }
                mqtt_server_port = tmp_value;

                is_success = ini.put("MQTT","mqtt_server_port",mqtt_server_port);
                if(!is_success)
                {
                    printf("ERROR: ini.put MQTT mqtt_server_port failed.\n ");
                    return -1;
                }
				has_config = 1;
                break;
            case 'C': /* MQTT Client ID as login username */
                memset(mqtt_user,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(mqtt_user,"%s",Value);

                is_success = ini.put("MQTT","mqtt_user",mqtt_user);
                if(!is_success)
                {
                    printf("ERROR: ini.put MQTT mqtt_user failed.\n ");
                    return -1;
                }
				has_config = 1;
                break;
            case 'D': /* MQTT client login password */
                memset(mqtt_passwd,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(mqtt_passwd,"%s",Value);

                is_success = ini.put("MQTT","mqtt_passwd",mqtt_passwd);
                if(!is_success)
                {
                    printf("ERROR: ini.put MQTT mqtt_passwd failed.\n ");
                    return -1;
                }
				has_config = 1;
                break;
            case 'E': /* FTP server IP address */
                memset(ftp_serverip,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(ftp_serverip,"%s",Value);

                is_success = ini.put("FTP","ftp_server_ip",ftp_serverip);
                if(!is_success)
                {
                    printf("ERROR: ini.put FTP ftp_server_ip failed.\n ");
                    return -1;
                }
				has_config = 1;
                break;
            case 'F': /* FTP server port */
                tmp_value =atoi(Value);
                if ((tmp_value<1)||(tmp_value>65535))
                {
                    printf("error: ftp port is invalid as %d.\n", tmp_value);
                    return -1;
                }
                ftp_server_port = tmp_value;

                is_success = ini.put("FTP","ftp_server_port",ftp_server_port);
                if(!is_success)
                {
                    printf("ERROR: ini.put FTP ftp_server_port failed.\n ");
                    return -1;
                }
				has_config = 1;
                break;
            case 'G': /* FTP login username */
                memset(ftp_user,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(ftp_user,"%s",Value);

                is_success = ini.put("FTP","ftp_user",ftp_user);
                if(!is_success)
                {
                    printf("ERROR: ini.put FTP ftp_user failed.\n ");
                    return -1;
                }
				has_config = 1;
                break;
            case 'H': /* FTP login password */
                memset(ftp_passwd,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(ftp_passwd,"%s",Value);

                is_success = ini.put("FTP","ftp_passwd",ftp_passwd);
                if(!is_success)
                {
                    printf("ERROR: ini.put FTP ftp_passwd failed.\n ");
                    return -1;
                }
                has_config = 1;
                break;
            case 'I': /* camera IP address */
                memset(camera_ip_addr,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(camera_ip_addr,"%s",Value);

                is_success = ini.put("CAMERA","camera_ip_addr",camera_ip_addr);
                if(!is_success)
                {
                    printf("ERROR: ini.put CAMERA camera_ip_addr failed.\n ");
                    return -1;
                }
				has_config = 1;
                break;
            case 'J': /* camera IP Network Mask */
                memset(camera_ip_mask,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(camera_ip_mask,"%s",Value);

                is_success = ini.put("CAMERA","camera_ip_mask",camera_ip_mask);
                if(!is_success)
                {
                    printf("ERROR: ini.put CAMERA camera_ip_mask failed.\n ");
                    return -1;
                }
				has_config = 1;
                break;
            case 'K': /* camera IP Gateway */
                memset(camera_ip_gateway,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(camera_ip_gateway,"%s",Value);

                is_success = ini.put("CAMERA","camera_ip_gateway",camera_ip_gateway);
                if(!is_success)
                {
                    printf("ERROR: ini.put CAMERA camera_ip_gateway failed.\n ");
                    return -1;
                }
				has_config = 1;
                break;
            case 'L': /* camera route */
		        tmp_value = atoi(Value);
                if ((tmp_value==0)||(tmp_value==180))
                {
                    camroute = tmp_value;

                    is_success = ini.put("CAMERA","camera_route",camroute);
                    if(!is_success)
                    {
                        printf("ERROR: ini.put CAMERA camera_route failed.\n ");
                        return -1;
                    }

                    break;
                }else{
                    printf("error camera route value:%d \n",camroute);
                    return -1;
                }
				has_config = 1;
            case 'M': /* camera flip */
		        tmp_value = atoi(Value);
                if ((tmp_value==0)||(tmp_value==1))
                {
                    camflip = tmp_value;

                    is_success = ini.put("CAMERA","camera_flip",camflip);
                    if(!is_success)
                    {
                        printf("ERROR: ini.put CAMERA camera_flip failed.\n ");
                        return -1;
                    }

                    break;
                }else{
                    printf("error camera flip value:%d \n",camflip);
                    return -1;
                }
				has_config = 1;
            default:
                printf("ERROR: this key %s key is doesn't be supported now.\n", temp);
                return -1;
        }
    }
	if(has_config == 0)
	{
		printf("no config changed! return!\n");
		return -1;
	}
	for(int p=0; p <= 3; p++)
	{
		eb_alarm(1);
		sleep(1);
		eb_alarm(0);
		sleep(1);
	}
	printf("device has configed,nned reboot!\n");
    system("reboot");
    return 0;
}

bool scan_image(cv::Mat &img_in)
{
	int ret;
	zbar::ImageScanner scanner;
	scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
	int width = img_in.cols;
	int height = img_in.rows;
	uchar *raw = (uchar *)img_in.data;
	zbar::Image image(width, height, "Y800", raw, width * height);
	int n = scanner.scan(image);
	zbar::Image::SymbolIterator symbol = image.symbol_begin();
	if(image.symbol_begin()==image.symbol_end())
	{
		return false;
	}
	for(;symbol != image.symbol_end();++symbol)
	{
		printf("get a qrcode: type %s ,data: %s \n",symbol->get_type_name().c_str(),\
		symbol->get_data().c_str());

		/* To save the QR code information into the configure file */
		ret = save_DevConfig(symbol->get_data().c_str());
		if(ret)
			return false;
	}
	return true;
}
