#ifndef CLOUD_SERVER_H
#define CLOUD_SERVER_H

#include<time.h>
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <vector>
#include <string>
#include <array>
#include <list>
#include <mutex>
#include <thread>

#include <functional>

#include<sys/types.h>
#include<sys/stat.h>
#include <string.h>
#include "mqtt_client.h"


#define MAX_LIST_SIZE  20
#define CMD_TYPE_FACEUP 0X70
#define CMD_TYPE_REGRET 0X71
#define CMD_TYPE_DOWN_REGFACE 0xa0
#define CMD_TYPE_DOWN_DELFACE 0xa1
#define CMD_TYPE_DOWN_SETPERM 0xa2
#define CMD_TYPE_DOWN_RESET 0xa3

#define IOT_CMD_DEVICE_CTL 0x0
#define IOT_CMD_DEVICE_REGISTER 0x08
#define IOT_CMD_DEVICE_DEL 0x09
#define IOT_CMD_DEVICE_AUTHER 0x0A
#define IOT_CMD_OTA 0xCC
#define IOT_CMD_DEVINFO 0xCA

#define NPT_TOPIC  "A/AT"

#define FTP_UPDATE_PATH "FtpUp"
#define FTP_UPDATE_OTA "OTA"
#define FTP_DOWN_PATH "FtpDownLoad"



/* upstream to publish */
typedef struct __UP_Dev_Status{
    uint8_t cmd_type;   //0
    uint8_t status;     //0:goodï¼Œ1:bad
}Up_Dev_Status;

typedef struct __UP_Recog_Result{
    uint8_t cmd_type;   //08
    uint8_t p_type;     //1-è¯†åˆ«æˆåŠŸï¼Œ2-é™Œç”Ÿäºº
    uint32_t p_id;      //äººå‘˜IDï¼ˆé™Œç”Ÿäººå¡«0å ä½å³å¯ï¼‰
    uint16_t year;      //å¹´
    uint8_t month;      //æœˆ
    uint8_t day;        //æ—¥
    uint8_t hour;       //æ—¶
    uint8_t min;        //åˆ†é’Ÿ
    uint8_t sec;        //ç§’
}Up_Recog_Result;

typedef struct __UP_User_Reg_Result{
    uint8_t cmd_type;   //09
    uint32_t p_id;      //äººå‘˜ID
    uint8_t result;     //ç™»è®°ç»“æœï¼š1æˆåŠŸï¼Œ 0å¤±è´¥
}Up_User_Reg_Result;

/* downstream to subscrible */
typedef struct __CTL_Dev{
    uint8_t cmd_type;   //0
    uint8_t status;     //1-é‡æ–°å¯åŠ¨è®¾å¤‡
}Ctl_Dev;

typedef struct __CTL_User_Reg{
    uint8_t cmd_type;   //08
    uint32_t p_id;      //äººå‘˜ID
    uint8_t floor;      //æ‰€åœ¨æ¥¼å±‚
    uint32_t pic_id;    //ç…§ç‰‡ID
}Ctl_User_Reg;

typedef struct __CTL_User_Del{
    uint8_t cmd_type;   //09
    uint32_t p_id;      //äººå‘˜ID
}Ctl_User_Del;

typedef struct __CTL_User_Auth{
    uint8_t cmd_type;   //0A
    uint32_t p_id;      //äººå‘˜ID
    uint8_t perm;       //1-å…è®¸é€šè¿‡ï¼Œ2-ä¸å…è®¸é€šè¿‡
}Ctl_User_Auth;

typedef struct __CTL_OTA{
	uint8_t cmd_type;  //IOT_CMD_OTA 0xCC
    uint8_t sw_version;   //Éı¼¶Éè±¸µÄÈí¼ş°æ±¾ÒªÇó
    uint8_t hw_version;   //Ó²¼ş°æ±¾ÒªÇó
    uint8_t for_all;  //ÊÇ·ñËùÓĞÉè±¸¶¼ÒªÉı¼¶å
    char ota_devid[20]; //½öÉı¼¶µ¥¶ÀÉè±¸Ê±Ö¸¶¨¸ÃÉè±¸µÄid
	char ota_file_name[20];// Õı³£Ó¦¸ÃÊÇ update.sh
}CTL_OTA;

typedef struct __WM_CTL_DEV_REG{
	uint8_t cmd_type; 
    uint8_t sw_version;   //08
    uint8_t hw_version;   //08
    char ota_devid[20];
}WM_CTL_DEV_REG;


// if need up
// make file
// set file name

typedef struct __Save_file{
	char file_name[50];
}Save_file;

class up_event{
public:
	int type;
    int32_t userlist[MAX_LIST_SIZE];
	uint8_t id_size;
	unsigned int year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
    char file_name[50];
	char user_data[10];
    up_event() : type(-1) ,id_size(-1){}
};

class uevent_mark{
public:
    int userid;
	uint8_t min;
	uint8_t sec;
    uevent_mark() : userid(-1),min(-1),sec(-1){}
};

class down_event{
public:
	int type;
    int32_t userid;
	uint8_t floor;
    char pic_name[50];
	char user_data[10];
    down_event() : type(-1),userid(-1), floor(0){}
};

#define MQTTC_CONNECTED  1
#define MQTTC_DISCONNECTED  0


class cloud_server
{
public:
	typedef std::function<int(down_event)> dEvent_Cb;
	cloud_server(char *uptopic,char* down_topic,char* ota_topic, uint8_t sw_version, uint8_t hw_version);
	~cloud_server();
	void ftp_init(char* serverip, char* username, char*passwd, int port);
	int mqttc_init(char* serverip,char* username, char* passwd, int portc, char *devid);
	void trigger_up(up_event &item);
	void trigger_down(void *data);
	void set_cmd_cb(dEvent_Cb cb);
	void ftp_up_trigger(char* filename);
	int ftpup(char* fname);
	void process_upevent();
	void process_downevent();
	int ftpdown(char* fname);
	void process_ftpup() ;
	bool has_marked(int userid);
	bool Is_need_up(up_event *event);
	void check_timeout(up_event &c_event);
	void mark_face(up_event &item);
	bool publish_dev_info(uint8_t sw_version, uint8_t hw_version,char* devid);

	bool publish_device_status(bool device_status);
	
	bool publish_recognize_result(uint8_t p_type, uint32_t p_id,
								  uint16_t year,uint8_t month,uint8_t day,uint8_t hour, uint8_t min, uint8_t sec);

	int zq_ota_ftpdown(char* fname);
	
	bool publish_user_register_result(uint32_t p_id, uint8_t result,uint8_t rsp_cmd);
	
	bool subscribe_device_control(const char *buffer);
	
	bool subscribe_user_register_or_modify(const char *buffer);
	
	bool subscribe_user_delete(const char *buffer);
	
	bool subscribe_user_auther(const char *buffer);
	
	bool mqttc_message_handler(MqttMsg *msg);
	
	bool ota_message_handler(MqttMsg *msg);

	bool subscribe_run_ota(const char *buffer);
	
	bool subscribe_run_zqota(const char *buffer);

	int ota_ftpdown(char* fname);

	void ota_ftp_init(char*, char*, char*, int);

	int ota_mqttc_init(char*, char*, char*, int, char*);
	bool publish_ota_heartbit(bool device_status);
	int ota_mqttc_reconnect();
	int mqttc_reconnect();
	bool zq_publish_dev_info(uint8_t sw_version, uint8_t hw_version,char* devid);
	int zq_updatelog();
	int check_otafile();
	
	bool start_flag;
	std::thread up_loop;
	std::thread down_loop;
	std::thread ftp_up_loop;
	mqtt_client *mqttc;
	mqtt_client *ota_mqttc;
	std::list<up_event> up_fifo;
	std::list<down_event> down_fifo;
	std::list<uevent_mark> mark_list;
	std::list<Save_file *> ftp_up_fifo;
	uint8_t sw_ver;
	uint8_t hw_ver;
	std::mutex mUp_Mutex;
	std::mutex mFtpUp_Mutex;
	std::mutex mDown_Mutex;
	dEvent_Cb down_event_cb;
	int ftp_init_flag;
	int ota_ftp_init_flag;
	int mqtt_init_flag;
	int ota_mqtt_init_flag;
	char mqtt_topic_up[50];
	char mqtt_topic_down[50];
	char mqtt_topic_ota[50];
	char ftpcmd[255];
	char OTA_ftpcmd[255];
	char OTA_dev_id[20];
	char ZQ_OTA_dev_id[20];
};
#endif
