#include "cloud_server.h"
cloud_server::cloud_server(char *uptopic,char* down_topic,char* ota_topic, uint8_t sw_version, uint8_t hw_version){
    start_flag = true;
	ftp_init_flag = 0;
	mqtt_init_flag = 0;
	memset(mqtt_topic_up,0,50);
	memset(mqtt_topic_down,0,50);
	sprintf(mqtt_topic_up,"%s",uptopic);
	sprintf(mqtt_topic_down,"%s",down_topic);
	sprintf(mqtt_topic_ota,"%s",ota_topic);
	up_loop=std::thread(&cloud_server::process_upevent,this);
	up_loop.detach();

	down_loop=std::thread(&cloud_server::process_downevent,this);
	down_loop.detach();

	ftp_up_loop = std::thread(&cloud_server::process_ftpup,this);
	ftp_up_loop.detach();

	sw_ver = sw_version;
	hw_ver = hw_version;
}

void cloud_server::ftp_init(char* serverip, char* username, char* passwd, int portc){
	memset(ftpcmd,0,255);
	sprintf(ftpcmd,"-u %s -p %s -P %d %s ",username,passwd,portc,serverip);
	ftp_init_flag = 1;
}

void cloud_server::ota_ftp_init(char* serverip, char* username, char* passwd, int portc){
	memset(OTA_ftpcmd,0,255);
	sprintf(OTA_ftpcmd,"-u %s -p %s -P %d %s ",username,passwd,portc,serverip);
	ota_ftp_init_flag = 1;
}


int cloud_server::mqttc_init(char* serverip,char* username, char* passwd, int portc, char *devid)
{
	printf("mqttc_init ++ \n");
	mqttc = new mqtt_client(serverip,portc,username,passwd,devid,1884);
	if(mqttc->connect() == 0){
		mqtt_init_flag = 1;
		printf("mqttc connect ok!\n");
		zq_publish_dev_info(sw_ver,hw_ver,devid);
	}else{
		printf("mqttc connect failed!\n");
	}
	memset(ZQ_OTA_dev_id,0,20);
	sprintf(ZQ_OTA_dev_id,"%s",devid);
	if(mqtt_init_flag == 1){
		mqttc->set_sub_callback(std::bind(&cloud_server::mqttc_message_handler, this, std::placeholders::_1));
		mqttc->subscribe(mqtt_topic_down,1);
		mqttc->subscribe(NPT_TOPIC,1);
	}

	return mqtt_init_flag;
}

int cloud_server::ota_mqttc_init(char* serverip,char* username, char* passwd, int portc, char *devid)
{
	printf("ota_mqttc_init ++ \n");
	ota_mqttc = new mqtt_client(serverip,portc,username,passwd,devid,1883);
	if(ota_mqttc->connect() == 0){
		ota_mqtt_init_flag = 1;
		printf("ota mqttc connect ok!\n");		
		publish_dev_info(sw_ver,hw_ver,devid);
	}else{
		printf("ota mqttc connect failed!\n");
	}

    memset(OTA_dev_id,0,20);
	sprintf(OTA_dev_id,"%s",devid);
	if(ota_mqtt_init_flag == 1){
		ota_mqttc->set_sub_callback(std::bind(&cloud_server::ota_message_handler, this, std::placeholders::_1));
		ota_mqttc->subscribe(mqtt_topic_ota,1);
	}

	return ota_mqtt_init_flag;
}


int cloud_server::mqttc_reconnect()
{
	if(mqttc->connect() == 0){
		printf("zq mqttc connect ok!\n");		
		mqttc->set_sub_callback(std::bind(&cloud_server::mqttc_message_handler, this, std::placeholders::_1));
		mqttc->subscribe(mqtt_topic_down,1);
		mqttc->subscribe(NPT_TOPIC,1);
		zq_publish_dev_info(sw_ver,hw_ver,ZQ_OTA_dev_id);
	}else{
		printf("zq mqttc connect failed!\n");
	}
	return 0;
}



int cloud_server::ota_mqttc_reconnect()
{
	if(ota_mqttc->connect() == 0){
		printf("ota mqttc connect ok!\n");		
		ota_mqttc->set_sub_callback(std::bind(&cloud_server::ota_message_handler, this, std::placeholders::_1));
		ota_mqttc->subscribe(mqtt_topic_ota,1);
		publish_dev_info(sw_ver,hw_ver,OTA_dev_id);
	}else{
			printf("ota mqttc connect failed!\n");
	}
	return 0;
}




#define FILTER_SECOND 30
void cloud_server::check_timeout(up_event &c_event)
{
	bool time_out=true;
	int list_size = 0;
	//remove time out mark
	list_size = mark_list.size();
	for(int i =0;i<list_size;i++)
	{
		uevent_mark item = mark_list.front();	
		if((item.min ==  c_event.min) \
		/*	&&(item.timest.tm_mday ==  t->tm_mday) \
			&&(item.timest.tm_hour ==  t->tm_hour) \
			&&(item.timest.tm_year ==  t->tm_year)\ */
			)
		{
			if((c_event.sec - item.sec) < FILTER_SECOND)
			{
				time_out = false;
			}
		}
		if(time_out){
			mark_list.pop_front();			
			printf("time_out  ! id %d sec %d \n",item.userid,item.sec);
		}
	}


}

bool cloud_server::has_marked(int userid)
{
	bool marked_flag = false;
	int list_size = 0;

	std::list<uevent_mark>::iterator item; //声明一个迭代器
	for(item = mark_list.begin();item!=mark_list.end();item++){
		if(item->userid == userid){
			marked_flag = true;
		}
	}

/*
	list_size = mark_list.size();
	for(int i =0;i<list_size;i++)
	{
		uevent_mark item = mark_list.front();
		if(item.userid == userid){
			marked_flag = true;
		}
	}*/
	return marked_flag;
}

bool cloud_server::Is_need_up(up_event *event)
{
	bool need = false;
	check_timeout(*event);
	for(int i =0;i<event->id_size;i++)
	{
		 if(!has_marked(event->userlist[i])){
		 	printf("new face %d \n",event->userlist[i]);
		 	need = true;
		 }else{
			event->userlist[i]= -1;
		 }
	}
	
	return need;
}

void cloud_server::trigger_up(up_event &item){
	std::unique_lock<std::mutex> locker(mUp_Mutex);
	if(up_fifo.size() < 10)
		up_fifo.push_back(item);
}


void cloud_server::trigger_down(void *data){
	down_event *item = (down_event *)data;
	std::unique_lock<std::mutex> locker(mDown_Mutex);
	down_fifo.push_back(*item);
}

void cloud_server::mark_face(up_event &item)
{	
	for(int i =0;i<item.id_size;i++)
	{
		if(!has_marked(item.userlist[i])){
			uevent_mark mark_item;
			mark_item.userid = item.userlist[i];
			mark_item.min =  item.min;
			mark_item.sec = item.sec;
			mark_list.push_back(mark_item);
			printf("mark_face  ! id %d sec %d \n",mark_item.userid,mark_item.sec);
		}
	}
}

void cloud_server::ftp_up_trigger(char* filename)
{	
    std::unique_lock<std::mutex> locker(mFtpUp_Mutex);
	if(ftp_up_fifo.size() < 10){
		Save_file *fs = (Save_file *)malloc(sizeof(Save_file));
		memcpy(fs->file_name,filename,50);
		ftp_up_fifo.push_back(fs);
	}else{
		printf("network is not good or ftp server is invalid !!\n");
	}
}

int cloud_server::ftpup(char* fname){
     int ret=0;
	 static int system_run=0;

	if(ftp_init_flag){
		char cmd_buf[255];
		memset(cmd_buf,0,255);
		sprintf(cmd_buf,"ftpput %s %s %s/%s",ftpcmd,fname,FTP_UPDATE_PATH,fname);
		if(system_run < 10){
			system_run++;
			ret = system(cmd_buf);
			system_run --;
			/*if(ret != 0)
				printf("ftp up file fialed %s %d \n",cmd_buf,ret);
			else
				printf("ftp up file successfully %s %d \n",cmd_buf,ret);
			*/
			char del_cmd_buf[100];
			memset(del_cmd_buf,0,100);
			sprintf(del_cmd_buf,"rm FtpUp/%s",fname);
			system(del_cmd_buf);
		}else{
			printf("update thread is too many need skip, network is not good or ftp is invalid !\n");
		}
	}
	return ret;
}

int cloud_server::ftpdown(char* fname){
     int ret=0;
	if(ftp_init_flag){
		char cmd_buf[255];
		memset(cmd_buf,0,255);
		sprintf(cmd_buf,"ftpget %s %s/%s %s",ftpcmd,FTP_DOWN_PATH,fname,fname);
		ret = system(cmd_buf);
		/*if(ret != 0)
			printf("ftp download file fialed %s \n",cmd_buf);
		else
			printf("ftp download file successfully %s \n",cmd_buf);
		*/
	}
	return ret;
}

int cloud_server::zq_updatelog()
{
	int ret =0;
	if(ftp_init_flag){
		char cmd_buf[255];
		char name_buf[100];
		memset(cmd_buf,0,255);
		memset(name_buf,0,100);
		struct tm *t;
		time_t tt;
		time(&tt);
		t = localtime(&tt);
		sprintf(name_buf,"%s%4d%02d%02d%02d%02d%02d.log",ZQ_OTA_dev_id,t->tm_year + 1900,t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		memset(cmd_buf,0,255);
		sprintf(cmd_buf,"ftpput %s %s system.log",ftpcmd,name_buf);
		ret = system(cmd_buf);
		printf("%s !\n",cmd_buf);
	}else{
		printf("net connect!\n");
	}
}

#if 1
char de_encrypt[6]={0x11,0x13,0x15,0x16,0x17,0x18};
int cloud_server::check_otafile()
{
	int fd_temp = -1;
	int len = 0;
	int i = 0;
	int j = 0;
	printf("check_otafile !\n");
	if(access("OTA/update.sh", F_OK) == 0){
	fd_temp = open("OTA/update.sh", O_RDONLY);
	if (fd_temp < 0)
	{
		printf("open OTA/update.sh error!\n");
		return 0;
	}
	int len = lseek(fd_temp, 0, SEEK_END);
	printf("buf len is 0x%x len is 0x%x \n",len*sizeof(unsigned int)/4+1,len);
	lseek(fd_temp, 0, SEEK_SET);
	char* ota_buf = (char*)malloc(len);
	read(fd_temp,(char *)(ota_buf), len);
	close(fd_temp);

	for(i = 0;i<len;i++)
	{
		ota_buf[i] = ota_buf[i] - de_encrypt[j];
		j++;
		if(j==6)
			j = 0;
	}
	fd_temp =-1;
	system("rm OTA/update.sh");
	int fd_temp = open("OTA/update.sh", O_RDWR|O_CREAT);
	if(fd_temp <0)
	{
		printf("open OTA/update.sh error!\n");
		return 0;
	}
	lseek(fd_temp, 0, SEEK_SET);
	write(fd_temp,(char *)(ota_buf), len);
	close(fd_temp);
	free(ota_buf);
	return 0;
 }	
	return -1;
}
#endif



int cloud_server::ota_ftpdown(char* fname){
     int ret=0;
	if(ftp_init_flag){
		char cmd_buf[255];
		memset(cmd_buf,0,255);
		sprintf(cmd_buf,"ftpget %s %s/%s %s",OTA_ftpcmd,FTP_UPDATE_OTA,"Vupdate.sh",fname);
		ret = system(cmd_buf);
	}
	return ret;
}

int cloud_server::zq_ota_ftpdown(char* fname){
     int ret=0;
	if(ftp_init_flag){
		char cmd_buf[255];
		memset(cmd_buf,0,255);
		sprintf(cmd_buf,"ftpget %s %s/%s %s",ftpcmd,FTP_UPDATE_OTA,"update.sh",fname);
		ret = system(cmd_buf);
		check_otafile();
	}
	return ret;
}



void cloud_server::process_ftpup() {
	int event_size;
	while(start_flag)
	{	
	    usleep(1000*50);
		event_size = ftp_up_fifo.size();
		if(event_size > 0){
			Save_file *item =NULL;
			{
				std::unique_lock<std::mutex> locker(mFtpUp_Mutex);
				item = ftp_up_fifo.front();
			}
			if(item != NULL){
				    ftpup(item->file_name);
					ftp_up_fifo.pop_front();
					free(item);
				/*
				if(ftpup(item->file_name) == 0){
					ftp_up_fifo.pop_front();
					free(item);
				}*/
		   }
		}
	}

}

void cloud_server::process_upevent() {
	int event_size;
	printf("start process_upevent \n");
	while(start_flag)
	{
		usleep(1000*50);
	    event_size = up_fifo.size();
		if(event_size > 0){
		printf("process_upevent event_size %d \n",event_size);
		std::unique_lock<std::mutex> locker(mUp_Mutex);
			for(int i = 0; i<event_size; i++)
			{
				up_event item = up_fifo.front();
				up_fifo.pop_front();
				switch(item.type) {
				case CMD_TYPE_FACEUP:
						for(int j=0;j<item.id_size;j++)
						{
							if(item.userlist[j] != -1)
					    		publish_recognize_result(0x1,item.userlist[j],item.year,item.month,item.date,item.hour,item.min,item.sec);
						}				
					break;
				//case CMD_TYPE_REGRET:
				//	publish_user_register_result(item.userlist[0],item.user_data[0]);
				//	break;
				default:
					break;
				}
			}
	    }
	}
}

void cloud_server::set_cmd_cb(dEvent_Cb cb)
{
	down_event_cb = cb;
}
void cloud_server::process_downevent(){
	int event_size;
	printf("start process_downevent \n");
	while(start_flag)
	{
		usleep(1000*50);
	    event_size = down_fifo.size();
		if(event_size > 0){
			printf("process_downevent event_size %d \n",event_size);
			std::unique_lock<std::mutex> locker(mDown_Mutex);
			for(int i = 0; i<event_size; i++)
			{
				down_event item = down_fifo.front();
				down_fifo.pop_front();
				int ret = down_event_cb(item);
				switch(item.type) {
				case CMD_TYPE_DOWN_REGFACE:
					if(!ret){
						publish_user_register_result(item.userid,0x1,IOT_CMD_DEVICE_REGISTER);
					}else{
						publish_user_register_result(item.userid,0x0,IOT_CMD_DEVICE_REGISTER);
					}
					break;
				case CMD_TYPE_DOWN_DELFACE:
					if(!ret){
						publish_user_register_result(item.userid,0x1,IOT_CMD_DEVICE_DEL);
					}else{
						publish_user_register_result(item.userid,0x0,IOT_CMD_DEVICE_DEL);
					}
					break;
				case CMD_TYPE_DOWN_SETPERM:
					if(!ret){
						publish_user_register_result(item.userid,0x1,IOT_CMD_DEVICE_AUTHER);
					}else{
						publish_user_register_result(item.userid,0x0,IOT_CMD_DEVICE_AUTHER);
					}
					break;
				case CMD_TYPE_DOWN_RESET:
					break;
				default:
					printf("error cmd type %d \n",item.type);
					break;
				}
			}
	    }
	}

}


extern void print_hex(const char *buffer, int size);

/* functions */
bool cloud_server::publish_device_status(bool device_status)
{
	char *publish_buffer = (char *)malloc(2);
	*publish_buffer = 0x0;
	*(publish_buffer+1) = device_status;

	mqttc->publish("UP/CAM/ZQ", publish_buffer, 2, 1);
	return true;
}


bool cloud_server::publish_ota_heartbit(bool device_status)
{
	char *publish_buffer = (char *)malloc(2);
	*publish_buffer = 0x0;
	*(publish_buffer+1) = device_status;

	ota_mqttc->publish("UP/CAM/OTA", publish_buffer, 2, 1);
	return true;
}


bool cloud_server::publish_recognize_result(uint8_t p_type, uint32_t p_id,
                              uint16_t year,uint8_t month,uint8_t day,uint8_t hour, uint8_t min, uint8_t sec)
{
	const char cmd_type = 0x8;
	uint32_t p_id_net = htonl(p_id);
	uint16_t year_net = htons(year);
	char *publish_buffer = (char *)malloc(13);
	memcpy(publish_buffer, &cmd_type, 1);
	memcpy(publish_buffer+1, &p_type, 1);
	memcpy(publish_buffer+2, &p_id_net, 4);
	memcpy(publish_buffer+6, &year_net, 2);
	memcpy(publish_buffer+8, &month, 1);
	memcpy(publish_buffer+9, &day, 1);
	memcpy(publish_buffer+10, &hour, 1);
	memcpy(publish_buffer+11, &min, 1);
	memcpy(publish_buffer+12, &sec, 1);

	mqttc->publish("UP/CAM/ZQ", publish_buffer, 13, 1);
	return true;
}

bool cloud_server::publish_user_register_result(uint32_t p_id, uint8_t result,uint8_t rsp_cmd)
{
	char cmd_type = 0x9;
	uint32_t p_id_net = htonl(p_id);
	char *publish_buffer = (char *)malloc(6);
	memcpy(publish_buffer, &cmd_type, 1);
	memcpy(publish_buffer+1, &p_id_net, 4);
	memcpy(publish_buffer+5, &rsp_cmd, 1);
	memcpy(publish_buffer+6, &result, 1);

	mqttc->publish("UP/CAM/ZQ", publish_buffer, 7, 1);
	return true;
}

bool cloud_server::publish_dev_info(uint8_t sw_version, uint8_t hw_version,char* devid)
{
	char cmd_type = IOT_CMD_DEVINFO;
	char *publish_buffer = (char *)malloc(sizeof(WM_CTL_DEV_REG));
	memset(publish_buffer,0,sizeof(WM_CTL_DEV_REG));
	memcpy(publish_buffer, &cmd_type, 1);
	memcpy(publish_buffer+1, &sw_version, 1);
	memcpy(publish_buffer+2, &hw_version, 1);
	sprintf(publish_buffer+3,"%s",devid);
	ota_mqttc->publish("UP/CAM/OTA", publish_buffer, sizeof(WM_CTL_DEV_REG), 1);
	return true;
}


bool cloud_server::zq_publish_dev_info(uint8_t sw_version, uint8_t hw_version,char* devid)
{
	char cmd_type = IOT_CMD_DEVINFO;
	char *publish_buffer = (char *)malloc(sizeof(WM_CTL_DEV_REG));
	memset(publish_buffer,0,sizeof(WM_CTL_DEV_REG));
	memcpy(publish_buffer, &cmd_type, 1);
	memcpy(publish_buffer+1, &sw_version, 1);
	memcpy(publish_buffer+2, &hw_version, 1);
	sprintf(publish_buffer+3,"%s",devid);
	mqttc->publish("UP/CAM/ZQ", publish_buffer, sizeof(WM_CTL_DEV_REG), 1);
	return true;
}


bool cloud_server::subscribe_device_control(const char *buffer)
{
	Ctl_Dev *ctl = (Ctl_Dev *)buffer;
	printf("device_contol:Status- %d", ctl->status);
	down_event event;
	event.type = CMD_TYPE_DOWN_RESET;
	event.user_data[0] = ctl->status;
	std::unique_lock<std::mutex> locker(mDown_Mutex);
	down_fifo.push_back(event);
	return ctl->status;
}

int security_level = 0;

bool cloud_server::subscribe_device_security_level(const char *buffer)
{
	Ctl_Dev_Security_Level *sec_level = (Ctl_Dev_Security_Level *)buffer;
	security_level = sec_level->level;
	printf("device_security_level:Level- %d", sec_level->level);
	return true;
}

bool cloud_server::subscribe_user_register_or_modify(const char *buffer)
{
	Ctl_User_Reg user;
	memcpy(&(user.p_id), buffer+1, 4);
	memcpy(&(user.floor), buffer+5, 1);
	memcpy(&(user.pic_id), buffer+6, 4);
	down_event event;
	event.type = CMD_TYPE_DOWN_REGFACE;
	event.floor = user.floor;
	event.userid = ntohl(user.p_id);
	sprintf(event.pic_name,"%d.jpg",ntohl(user.pic_id));
	std::unique_lock<std::mutex> locker(mDown_Mutex);
	down_fifo.push_back(event);

	printf("user_register:ID- %d, Floor- %d, PIC_ID- %d\n", ntohl(user.p_id), user.floor, ntohl(user.pic_id));

	return true;
}


bool cloud_server::subscribe_run_ota(const char *buffer)
{
	CTL_OTA ota_data;
	memcpy(&(ota_data.sw_version), buffer+1, 1);
	memcpy(&(ota_data.hw_version), buffer+2, 1);
	memcpy(&(ota_data.for_all), buffer+3, 1);
	memcpy(&(ota_data.ota_devid), buffer+4, 20);
	memcpy(&(ota_data.ota_file_name), buffer+24, 20);
	printf("swver- %d, hwver- %d, otafile- %s\n",ota_data.sw_version,ota_data.hw_version, ota_data.ota_file_name);
    if((sw_ver == ota_data.sw_version)&&(ota_data.hw_version == hw_ver))
    {
    	if(ota_data.for_all == 1){
			printf("system need update!");
			ota_ftpdown(ota_data.ota_file_name);
			system("reboot");
    	}else if(!strcmp(OTA_dev_id,ota_data.ota_devid))
    	{
			printf("system need update!");
			ota_ftpdown(ota_data.ota_file_name);
			system("reboot");
		}else{
			printf("update is not for this device!\n");
		}
	}
	return true;
}


bool cloud_server::subscribe_run_zqota(const char *buffer)
{
	CTL_OTA ota_data;
	memcpy(&(ota_data.sw_version), buffer+1, 1);
	memcpy(&(ota_data.hw_version), buffer+2, 1);
	memcpy(&(ota_data.for_all), buffer+3, 1);
	memcpy(&(ota_data.ota_devid), buffer+4, 20);
	memcpy(&(ota_data.ota_file_name), buffer+24, 20);
	printf("swver- %d, hwver- %d, otafile- %s\n",ota_data.sw_version,ota_data.hw_version, ota_data.ota_file_name);
    if((sw_ver == ota_data.sw_version)&&(ota_data.hw_version == hw_ver))
    {
    	if(ota_data.for_all == 1){
			if(!strcmp(ota_data.ota_file_name,"getlog"))
			{
				printf("zq get log!\n");
				zq_updatelog();
			}else if(!strcmp(ota_data.ota_file_name,"cleanlog"))
			{
				printf("zq clean log!\n");
				system("echo start > system.log");
			}else{
				printf("zq system need update!");
				zq_ota_ftpdown(ota_data.ota_file_name);
				system("reboot");
			}
    	}else if(!strcmp(ZQ_OTA_dev_id,ota_data.ota_devid))
    	{
			if(!strcmp(ota_data.ota_file_name,"getlog"))
			{
				printf("zq get log!\n");
				zq_updatelog();
			}else if(!strcmp(ota_data.ota_file_name,"cleanlog"))
			{
				printf("zq clean log!\n");
				system("echo start > system.log");
			}else{
				printf("zq system need update!");
				zq_ota_ftpdown(ota_data.ota_file_name);
				system("reboot");
			}
		}else{
			printf("zq update is not for this device!\n");
		}
	}
	return true;
}


bool cloud_server::subscribe_user_delete(const char *buffer)
{
        Ctl_User_Del user;
	    memcpy(&(user.p_id),buffer+1,4);
		down_event event;
		event.type = CMD_TYPE_DOWN_DELFACE;
		event.userid = ntohl(user.p_id);
		std::unique_lock<std::mutex> locker(mDown_Mutex);
		down_fifo.push_back(event);

        printf("user_delete: ID- %d\n", ntohl(user.p_id));

        return true;
}

bool cloud_server::subscribe_user_auther(const char *buffer)
{
        Ctl_User_Auth user;
		memcpy(&(user.p_id),buffer+1,4);
		memcpy(&(user.perm),buffer+5,1);
		
		down_event event;
		event.type = CMD_TYPE_DOWN_SETPERM;
		event.userid = ntohl(user.p_id);
		event.user_data[0] = user.perm;
		
		std::unique_lock<std::mutex> locker(mDown_Mutex);
		down_fifo.push_back(event);

        printf("user_auther: ID- %d, permit- %d\n", ntohl(user.p_id), user.perm);

        return true;
}

bool cloud_server::mqttc_message_handler(MqttMsg *msg)
{
	bool ret = false; 
	uint8_t cmd_type = *((char *)(msg->payload));
	printf("cmd_type: %d\n", cmd_type);
	switch(cmd_type)
	{
		case IOT_CMD_DEVICE_CTL:
			ret = subscribe_device_control(msg->payload);
			break;
		case IOT_CMD_DEVICE_SECURITY_LEVEL:
			ret = subscribe_device_security_level(msg->payload);
			break;
		case IOT_CMD_DEVICE_REGISTER:
			ret = subscribe_user_register_or_modify(msg->payload);
			break;
		case IOT_CMD_DEVICE_DEL:
			ret = subscribe_user_delete(msg->payload);
			break;
		case IOT_CMD_DEVICE_AUTHER:
			ret = subscribe_user_auther(msg->payload);
			break;
		case IOT_CMD_OTA:
			ret = subscribe_run_zqota(msg->payload);
			break;	
		default:
			printf("ERROR: cmd_type not recognized.\n");
			break;
	}
	return ret;
}

bool cloud_server::ota_message_handler(MqttMsg *msg)
{
	bool ret = false; 
	uint8_t cmd_type = *((char *)(msg->payload));
	printf("ota cmd_type: %d\n", cmd_type);
	switch(cmd_type)
	{
		case IOT_CMD_OTA:
			ret = subscribe_run_ota(msg->payload);
			break;
		default:
			printf("ERROR: cmd_type not recognized.\n");
			//ret = true;
			break;
	}
	return ret;
}


