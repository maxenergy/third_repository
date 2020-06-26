#include "main.h"
#include "sample_comm.h"
#include "mpi_awb.h"
extern "C" {
extern int sysinit(int *fd_out,int mroute,int mflip);
extern void log_level_set(int level);
extern void init_pola_sdk(int liveless_mode,float detect_threshold,int test_flag);
extern  HI_VOID SAMPLE_COMM_VENC_RegCallback(VENC_CHN vencChn, SAMPLE_VENC_CALLBACK_S *callBack);
}

#define GPIO_IR_LED_IN      2
#define GPIO_MIR_S0 		4
#define GPIO_MIR_S1 		5

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


void set_time()
{
	//system("ntpclient -s -d -c 1 -i 5 -h 202.112.10.60");
	//do nothing
}

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
	eth0_ifconfig();
	vendor_checkota();
	setup_env();
	
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

	wdt_loop = std::thread(&thread_wdt_loop);
    wdt_loop.detach();
	checkota();
	ota_stoped = 1;
	
	setup_env();
	eth0_ifconfig();
	set_time();

    tcp_cmd = new Tcp_Cmd(TCP_CMD_PORT,update_face,tcp_ota_func);
	int fd_sys;
	std::cout << "pola sdk init!!! " << std::endl;
	SAMPLE_VENC_CALLBACK_S callBack;
	callBack.pfnDataCB = venc_h264_callback;
	sysinit(&fd_sys,camroute,camflip);
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
	
 //   Logger::setLogLevel(Logger::LogWarning);
    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);
 
    ThreadPool* threadPool = ThreadPool::createNew(2);	
    env = UsageEnvironment::createNew(scheduler, threadPool);
     Ipv4Address ipAddr("0.0.0.0", 554);
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
		sleep(1);
		VIFrame mBtnPhoto;
		FaceRecognitionApi::getInstance().capture(1,mBtnPhoto);
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
	sprintf(item->file_name,"%s%4d%02d%02d%02d%02d%02d.jpg",g_device_sn,t->tm_year + 1900,t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}
void save_pic(char* name)
{
	VIFrame mBtnPhoto;
	char full_name[255];
	//unsigned char* rgb_buf;
	FaceRecognitionApi::getInstance().capture(2,mBtnPhoto);
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
	printf("systime is  %4d-%02d-%02 d%02:d%02d:%02d \n",t->tm_year + 1900,t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	#else
	printf("systime is  %4d-%02d-%02d d%02:d%02d:%02d \n",t->tm_year + 1900,t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	#endif
}
	
#define MQTT_PING_TIME_PRESEC  50

void thread_aiot_setup()
{
	int ret =0;
	ret = Aiot_server->mqttc_init(mqtt_server_ip,mqtt_user, mqtt_passwd, mqtt_server_port, g_device_sn);
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
			Aiot_server->zq_updatelog();
			sleep(20);
			system("echo start > system.log");
		}
		sleep(LOG_TIME_PRESEC);
	}
}

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
	printf("info.floor is %d \n",info.floor);
	prs485->ctl_sw(0,info.floor-1,1);
	prs485->ctl_sw(0,0,1);
	//prs485->ctl_sw(0,1,1);
	//prs485->ctl_relay(0,onoff);
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
		if(wdt_alive_flag) {
			wdt_alive_flag = 0;
			ret = ioctl(fd, WDIOC_KEEPALIVE);
		}else{
			if(ota_stoped == 0)
				ret = ioctl(fd, WDIOC_KEEPALIVE);
			else
				printf("vdec has no data!\n need reboot!\n");
		}
	}
}
int param_check(char* str)
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
	sprintf(ifcmd_buf,"route add default gw %s", camera_ip_gateway);
	system(ifcmd_buf);
}

void setup_env()
{
	int index = 0;
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
    for(int i=0;i<6;i++)
    {
		hw_addr[i] = ifreq.ifr_hwaddr.sa_data[i];
	}
	memset(fixed_dev_sn,0,20);
    sprintf(fixed_dev_sn,"%s_%x%x%x%x%x%x_%d",CUSTOM_ID,hw_addr[0],hw_addr[1], \
		hw_addr[2],hw_addr[3],hw_addr[4],hw_addr[5],DEV_TYPE);

	printf("fixed_dev_sn %s \n",fixed_dev_sn);
	//close(sock);
	
	if(access("dev_config", F_OK) == 0)
	{
		int fd;
		char data[1024] = {0};
		int i;

		fd = open("dev_config", O_RDONLY);
		if (fd < 0)
		{
			perror("file open config file!\n ");    
			return;
		}

		read(fd, data, 1024);

		
		char*temp = strtok(data,",");
		while(temp)
		{
			switch(index)
			{
				case PARAM_INDEX_CAM_ROUTE:
					 camroute =atoi(temp);
					 printf("camroute is %d \n",camroute);
					 break;

				case PARAM_INDEX_CAM_FLIP:
					camflip =atoi(temp);
					printf("camflip is %d \n",camflip);
					break;
					
				case PARAM_INDEX_TESTFLAG:
					test_flag =atoi(temp);
					printf("test_flag is %d \n",test_flag);
					break;
					
				case PARAM_INDEX_MQTT_UPTOPIC:
					memset(mqtt_uptopic,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(mqtt_uptopic,"%s",temp);
					printf("mqtt_uptopic is %s \n",mqtt_uptopic);
					break;
				case PARAM_INDEX_MQTT_DOWNTOPIC:
					memset(mqtt_downtopic,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(mqtt_downtopic,"%s",temp);
					printf("mqtt_downtopic is %s \n",mqtt_downtopic);
					break;
				case PARAM_INDEX_FTP_SERVER_IP:
					memset(ftp_serverip,0,MAX_PARAM_SIZE);
				    if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}	
					sprintf(ftp_serverip,"%s",temp);
					printf("ftp_serverip is %s \n",ftp_serverip);
					break;
				case PARAM_INDEX_FTP_SERVER_PORT:
					ftp_server_port =atoi(temp);
					printf("ftp_server_port is %d \n",ftp_server_port);
					break;
				case PARAM_INDEX_FTP_USERNAME:
					memset(ftp_user,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ftp_user,"%s",temp);
					printf("ftp_user is %s \n",ftp_user);
					break;
				case PARAM_INDEX_FTP_PASSWD:
					memset(ftp_passwd,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ftp_passwd,"%s",temp);
					printf("ftp_passwd is %s \n",ftp_passwd);
					break;
					
				case PARAM_INDEX_MQTT_SERVER_IP:
					memset(mqtt_server_ip,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(mqtt_server_ip,"%s",temp);
					printf("mqtt_server_ip is %s \n",mqtt_server_ip);
					break;
					
				case PARAM_INDEX_MQTT_SERVER_PORT:
					mqtt_server_port = atoi(temp);
					printf("mqtt_server_port is %d \n",mqtt_server_port);
					break;
					
				case PARAM_INDEX_MQTT_USERNAME:
					memset(mqtt_user,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(mqtt_user,"%s",temp);
					printf("mqtt_user is %s \n",mqtt_user);
					break;
				case PARAM_INDEX_MQTT_PASSWD:
					memset(mqtt_passwd,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(mqtt_passwd,"%s",temp);
					printf("mqtt_passwd is %s \n",mqtt_passwd);
					break;
					
				case PARAM_INDEX_OTA_MQTT_DOWNTOPIC:
					memset(ota_mqtt_downtopic,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_mqtt_downtopic,"%s",temp);
					printf("ota_mqtt_downtopic is %s \n",ota_mqtt_downtopic);
					break;
					
				case PARAM_INDEX_OTA_MQTT_SERVER_IP:
					memset(ota_mqtt_server_ip,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_mqtt_server_ip,"%s",temp);
					printf("ota_mqtt_server_ip is %s \n",ota_mqtt_server_ip);
					break;
				case PARAM_INDEX_OTA_MQTT_SERVER_PORT:
					ota_mqtt_server_port = atoi(temp);
					printf("ota_mqtt_server_port is %d \n",ota_mqtt_server_port);
					break;
					
				case PARAM_INDEX_OTA_MQTT_USERNAME:
					memset(ota_mqtt_user,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_mqtt_user,"%s",temp);
					printf("ota_mqtt_user is %s \n",ota_mqtt_user);
					break;

				case PARAM_INDEX_OTA_MQTT_PASSWD:
					memset(ota_mqtt_passwd,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_mqtt_passwd,"%s",temp);
					printf("ota_mqtt_passwd is %s \n",ota_mqtt_passwd);
					break;
					
				case PARAM_INDEX_OTA_FTP_SERVER_IP:
					memset(ota_ftp_serverip,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_ftp_serverip,"%s",temp);
					printf("ota_ftp_serverip is %s \n",ota_ftp_serverip);
					break;
				
				case PARAM_INDEX_OTA_FTP_SERVER_PORT:
					ota_ftp_server_port = atoi(temp);
					printf("ota_ftp_server_port is %d \n",ota_ftp_server_port);
					break;

				case PARAM_INDEX_OTA_FTP_USERNAME:
					memset(ota_ftp_user,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_ftp_user,"%s",temp);
					printf("ota_ftp_user is %s \n",ota_ftp_user);
					break;
				
				case PARAM_INDEX_OTA_FTP_PASSWD:
					memset(ota_ftp_passwd,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(ota_ftp_passwd,"%s",temp);
					printf("ota_ftp_passwd is %s \n",ota_ftp_passwd);
					break;
				
				case PARAM_INDEX_CAMERA_IP_ADDR:
					memset(camera_ip_addr,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(camera_ip_addr,"%s",temp);
					printf("camera_ip_addr is %s \n",camera_ip_addr);
					break;

				case PARAM_INDEX_CAMERA_IP_MASK:
					memset(camera_ip_mask,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(camera_ip_mask,"%s",temp);
					printf("camera_ip_mask is %s \n",camera_ip_mask);
					break;

				case PARAM_INDEX_CAMERA_IP_GATEWAY:
					memset(camera_ip_gateway,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(camera_ip_gateway,"%s",temp);
					printf("camera_ip_gateway is %s \n",camera_ip_gateway);
					break;

				case PARAM_INDEX_G_DEVICE_SN:
					memset(g_device_sn,0,MAX_PARAM_SIZE);
					if(param_check(temp)){
						printf("error str len > maxsize \n",MAX_PARAM_SIZE);
						break;
					}
					sprintf(g_device_sn,"%s",temp);
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
			index++;
			temp = strtok(NULL,",");
		}

		close(fd);
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
    size_t size;
    char QRdata[1024] = {0};
    char dev_config_data[1024] = {0};

    /* judgement the security level */
    if (!security_level)
    {
        printf("WARNING: This security level %d isn't allowed to set dev_config", security_level);
	return 0;
    }

    /* remove the header: "INITCAM:" */
    config_buffer += 8;

    memset(QRdata, 0, 1024);
    memcpy(QRdata, config_buffer, strlen(config_buffer));

    /* analysis the config information from QR code. */
    char*temp = strtok(QRdata,";");
    while(temp)
    {
        char Key = (char)*temp;
        char *Value = temp+2;

        temp = strtok(NULL, ";");

        /* If Security_level is 1, only IP address could be set. */
        if (!(((security_level == 1)&&((Key=='I')||(Key=='J')||(Key=='K')))||(security_level==2)))
        {
            printf("Tips: Current Security Level is %d, can't set Key %c\n", security_level, Key);
            continue;
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
	        break;
            case 'B': /* MQTT server port */
                mqtt_server_port =atoi(Value);
                break;
            case 'C': /* MQTT Client ID as login username */
                memset(mqtt_user,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(mqtt_user,"%s",Value);
                break;
            case 'D': /* MQTT client login password */
                memset(mqtt_passwd,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(mqtt_passwd,"%s",Value);
                break;
            case 'E': /* FTP server IP address */
                memset(ftp_serverip,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(ftp_serverip,"%s",Value);
                break;
            case 'F': /* FTP server port */
                ftp_server_port =atoi(Value);
                break;
            case 'G': /* FTP login username */
                memset(ftp_user,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(ftp_user,"%s",Value);
                break;
            case 'H': /* FTP login password */
                memset(ftp_passwd,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(ftp_passwd,"%s",Value);
                break;
            case 'I': /* camera IP address */
                memset(camera_ip_addr,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(camera_ip_addr,"%s",Value);
                break;
            case 'J': /* camera IP Network Mask */
                memset(camera_ip_mask,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(camera_ip_mask,"%s",Value);
                break;
            case 'K': /* camera IP Gateway */
                memset(camera_ip_gateway,0,MAX_PARAM_SIZE);
                if(param_check(Value))
                {
                     printf("error str len > maxsize \n",MAX_PARAM_SIZE);
                     return -1;
                }
                sprintf(camera_ip_gateway,"%s",Value);
                break;
            default:
                printf("ERROR: this key %s doesn't be supported now.\n", temp);
                return -1;
        }
    }

    /* open the orignal config file, If not exist, create it. */
    fd = open("dev_config", O_RDWR|O_CREAT);
    if (fd < 0)
    {
        perror("file open config file!\n ");
        return -1;
    }

    /* Flush the orignal config file, new data will update it. */
    ftruncate(fd,0);
    lseek(fd,0,SEEK_SET);

    memset(dev_config_data, 0, 1024);
    /* assemble init information into dev_config file */
    sprintf(dev_config_data, "%d,%d,%d,%s,%s,%s,%d,%s,%s,%s,%d,%s,%s,%s,%s,%d,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%d,%d",
	     camroute,camflip,test_flag,mqtt_uptopic,mqtt_downtopic,ftp_serverip,ftp_server_port,ftp_user,ftp_passwd,
             mqtt_server_ip,mqtt_server_port,mqtt_user,mqtt_passwd,
             ota_mqtt_downtopic,ota_mqtt_server_ip,ota_mqtt_server_port,ota_mqtt_user,ota_mqtt_passwd,
             ota_ftp_serverip,ota_ftp_server_port,ota_ftp_user,ota_ftp_passwd,
             camera_ip_addr,camera_ip_mask,camera_ip_gateway,
             g_device_sn,sw_ver,hw_ver);

    /* write into dev_config file */
    size = write(fd, dev_config_data, strlen(dev_config_data));
    if(size != strlen(dev_config_data))
    {
        printf("save_InitVonfig: write file faild.\n");
	close(fd);
	return -1;
    }

    close(fd);
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
