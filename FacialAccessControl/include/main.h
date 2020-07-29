#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/if.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "sample_comm.h"
#include "minIni.h"
#include "watchdog.h"
#include "zbar.h"
#include "cloud_server.h"

#define WDT_DEV_FILE "/dev/watchdog"

#define QRCODE_PIC_CHN      2
#define OTA_PIC_CHN         1  //416 x 416
#define FACE_UPPATH         "FtpUp"

#define GPIO_IR_LED_IN      2
#define GPIO_MIR_S0         4
#define GPIO_MIR_S1         5
#define GPIO_EB_ALARM       84

#define CUSTOM_ID           "WXZQ"
#define DEV_TYPE            01

enum SECURITY_LEVEL{
        NOT_ALLOW_QRCODE=0,
        ONLF_IP_AND_CAM,
        FOR_ALL_CONFIG,
};

int security_level = FOR_ALL_CONFIG;
int wdt_alive_flag = 0;
int system_init_stoped =0;

cloud_server *aiot_server;


std::mutex mFifo_Mutex;

std::list<Save_file*> save_file_fifo;
std::thread save_file_loop;
std::thread wdt_loop;
std::thread ir_cut_thread;
std::thread qrcode_loop;
std::thread aiot_setup_once;

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

int test_flag;

