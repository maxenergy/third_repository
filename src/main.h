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
#include "tcp_cmd.h"
#include <opencv2/opencv.hpp>
#include "cloud_server.h"
#include "rs485.h"

#include <unistd.h>
#include "watchdog.h"
#include <sys/ioctl.h>
#include <dirent.h>
#include <net/if.h>
#include<signal.h>
#include <execinfo.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "zbar.h" 
#include <vector>

#define OTA_PIC_CHN   1  //416 x 416
#define QRCODE_PIC_CHN  2  // 960 x 540
#define XDETECT_PIC_CHN  1  // 416 x 416

//#define BUILD_SYSTEM_DEMO
#define WDT_DEV_FILE "/dev/watchdog"
#define FACE_UPPATH "FtpUp"
#define TCP_CMD_PORT  1314
#define MAX_INFO_SIZE 24
#define FACE_RECORD 0x11

//devid:custom_id+mac+devtype
#define CUSTOM_ID  "WXZQ"
#define DEV_TYPE 01
//customid:WXZQ
//devtype:01 - 02

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
	unsigned int userid;
}AI_Box;

typedef struct __AI_Data{
	uint8_t size;
    AI_Box * box;
	unsigned int timestamp;
}AI_Data;


enum SECURITY_LEVEL{
	NOT_ALLOW_QRCODE=0,
	ONLF_IP_AND_CAM,
	FOR_ALL_CONFIG,
};

enum param_index{
	PARAM_INDEX_CAM_ROUTE = 0,
	PARAM_INDEX_CAM_FLIP,
	PARAM_INDEX_TESTFLAG,
	PARAM_INDEX_MQTT_UPTOPIC,
	PARAM_INDEX_MQTT_DOWNTOPIC,
	PARAM_INDEX_FTP_SERVER_IP,
	PARAM_INDEX_FTP_SERVER_PORT,
	PARAM_INDEX_FTP_USERNAME,
	PARAM_INDEX_FTP_PASSWD,

	PARAM_INDEX_MQTT_SERVER_IP,
	PARAM_INDEX_MQTT_SERVER_PORT,
	PARAM_INDEX_MQTT_USERNAME,
	PARAM_INDEX_MQTT_PASSWD,
	
	PARAM_INDEX_OTA_MQTT_DOWNTOPIC,
	PARAM_INDEX_OTA_MQTT_SERVER_IP,
	PARAM_INDEX_OTA_MQTT_SERVER_PORT,
	PARAM_INDEX_OTA_MQTT_USERNAME,
	PARAM_INDEX_OTA_MQTT_PASSWD,
	
	PARAM_INDEX_OTA_FTP_SERVER_IP,
	PARAM_INDEX_OTA_FTP_SERVER_PORT,
	PARAM_INDEX_OTA_FTP_USERNAME,
	PARAM_INDEX_OTA_FTP_PASSWD,
	
	PARAM_INDEX_CAMERA_IP_ADDR,
	PARAM_INDEX_CAMERA_IP_MASK,
	PARAM_INDEX_CAMERA_IP_GATEWAY,

	PARAM_INDEX_G_DEVICE_SN,
	PARAM_INDEX_SW_VER,
	PARAM_INDEX_HW_VER
};

#define MAX_PARAM_SIZE 20

char fixed_dev_sn[MAX_PARAM_SIZE];

char mqtt_uptopic[MAX_PARAM_SIZE];
char mqtt_downtopic[MAX_PARAM_SIZE];

char ftp_serverip[MAX_PARAM_SIZE];
int ftp_server_port;
char ftp_user[MAX_PARAM_SIZE];
char ftp_passwd[MAX_PARAM_SIZE];

char  mqtt_server_ip[MAX_PARAM_SIZE];
int  mqtt_server_port;
char  mqtt_user[MAX_PARAM_SIZE];
char  mqtt_passwd[MAX_PARAM_SIZE];

char ota_mqtt_downtopic[MAX_PARAM_SIZE];

char ota_mqtt_server_ip[MAX_PARAM_SIZE];
int ota_mqtt_server_port;
char ota_mqtt_user[MAX_PARAM_SIZE];
char ota_mqtt_passwd[MAX_PARAM_SIZE];

char ota_ftp_serverip[MAX_PARAM_SIZE];
int ota_ftp_server_port;
char ota_ftp_user[MAX_PARAM_SIZE];
char ota_ftp_passwd[MAX_PARAM_SIZE];

char camera_ip_addr[MAX_PARAM_SIZE];
char camera_ip_mask[MAX_PARAM_SIZE];
char camera_ip_gateway[MAX_PARAM_SIZE];

char g_device_sn[MAX_PARAM_SIZE];

uint8_t sw_ver = 0x8;

uint8_t hw_ver = 0x2;

std::mutex data_mutex;
FaceRecognitionApi *framework;

UsageEnvironment* env;
RtspServer* server;
MediaSession* session;
RtpSink* rtpSink;
AI_Box FRI[32];
AI_Box XDI[32];

int box_count = 0;
int box_count_obj = 0;
unsigned long buf_index =0;
std::thread rtsp_push_loop;
unsigned char* loop_buf;
Tcp_Cmd * tcp_cmd;
int clean_flag = 0;
cloud_server *Aiot_server;

int last_sec = 0;
int last_min = 0;

int current_sec = 0;
int current_min = 0;
int wdt_alive_flag = 0;

int camroute = 0;
int camflip = 0;
int test_flag;
int system_init_stoped =0;
rs485_port *prs485;

std::mutex mFifo_Mutex;

std::list<Save_file*> save_file_fifo;

std::thread save_file_loop;

std::thread aiot_setup_once;

std::thread ir_cut_thread;

std::thread wdt_loop;

std::thread log_manager_loop;


std::thread ota_setup_once;

std::thread qrcode_loop;

int security_level = FOR_ALL_CONFIG;

void thread_ota_setup();

void thread_ircut();

void checkota();
void setup_env();
void register_sig();
void HandleSig(int signum);
void eth0_ifconfig();
int process_aiotevent(down_event event);
void thread_save_file();
void thread_wdt_loop();
void thread_aiot_setup();
void thread_qrcode_setup();
int  tcp_ota_func(char* cmd);
int update_face(Face_Upstream_First* cmd_buf);
void process_detectet(FaceDetect::Msg bob);
void vendor_checkota();
void thread_log_manager();
void process_xdetectet(FaceDetect::Msg bob);
bool scan_image(cv::Mat &img_in);
void gpio_write(int gpio,int on_off);


