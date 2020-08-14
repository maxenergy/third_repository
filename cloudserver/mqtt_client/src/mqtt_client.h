#ifndef __MQTT_CLIENT_H
#define __MQTT_CLIENT_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>

extern "C" {
#include "ae.h"
#include "anet.h"
#include "mqtt.h"
#include "zmalloc.h"
#include "packet.h"
}
#include <stdbool.h>

#define _NOTUSED(V) ((void)V)

static const char *PROMPT = "mqttc> ";

static const char *COMMANDS[3] = {
	"publish topic qos message\n",
	"subscribe topic qos\n",
	"unsubscribe topic\n"
};

typedef struct _Client {
	aeEventLoop *el;
	Mqtt *mqtt;
	bool shutdown_asap;
} Client;

// 1. 设备注册
// 2. 识别信息上传
// 3. 人员录入
// 4. 权限更新
// 5. 人员删除

#if 0
typedef struct _device_info {
	char device_name[20];
	char addr[20];
	int type;
}Device_Info;

typedef struct _frg_info{
	char file_path[50];
	char name[20];
	int userid;
}frg_info;


typedef struct _face_update {
	char file_path[50];
	char name[20];
	int userid;
	int permission; //floor << 8 | room_num
}Face_Update;
#endif



class mqtt_client
{
public:
	typedef std::function<bool(MqttMsg *)> subcbFunc;
	mqtt_client(char* server,int port, char* username, char* passwd, char *devid,int host_port);
	~mqtt_client();
	void subscribe(char* topic,unsigned char qos);
	void publish(char* topic , char* data,int size ,uint8_t qos);
	void client_init();
	void on_message(MqttMsg *msg);
	void on_disconnect(void *data, int id);
	void on_pingresp(void *data, int id);
	void on_pingreq(void *data, int id);
	void on_unsuback(void *data, int msgid);
	void on_unsubscribe(void *data, int msgid) ;
	void on_suback(void *data, int msgid);
	void on_subscribe(void *data, int msgid);
	void on_pubcomp(void *data, int msgid) ;
	void on_pubrel(void *data, int msgid);
	void on_pubrec(void *data, int msgid);
	void on_puback(void *data, int msgid);
	void on_publish(void *data, int msgid) ;
	void on_connack(void *data, int rc) ;
	void on_connect( void *data, int state);
	int client_cron(long long id, void *clientData);
	void client_setup(char* server,int port, char* username, char* passwd, int keeplive);
	void mqtt_init(Mqtt *mqtt);
	void set_sub_callback(subcbFunc cb);
	void set_callbacks(Mqtt *mqtt);
	int connect();
	int disconnect();
	bool HasConnected();
	void sendping();
	Client client;
	int connect_state;
	std::thread connect_loop;
	char clientid[50];
	subcbFunc subcb;
	int mhostp= 0;
	int pinrsp= 0;
	int pinreq = 0;
};

#endif //__MQTT_CLIENT_H
