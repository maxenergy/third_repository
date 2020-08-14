#include "mqtt_client.h"
#include "mqtt_message.h"

void print_hex(const char *buffer, int size)
{
        for(int i=0; i<size; i++)
        {
              printf("0x%02x ", *(unsigned char *)(buffer+i));
        }
        printf("\n");
}
mqtt_client::mqtt_client(char* server,int port, char* username, char* passwd, char * devid,int host_port)
{
	connect_state = MQTT_STATE_DISCONNECTED;
	memset(clientid,0,50);
	sprintf(clientid, devid);
	mhostp = host_port;
	subcb = NULL;
	client_init();
	//parse args
	client_setup(server, port,username,passwd,10);
	set_callbacks(client.mqtt);
	
	connect_loop=std::thread(mqtt_run,client.mqtt);
	connect_loop.detach();
	
	return;
}

int mqtt_client::connect()
{
	int time_out = 0;
	if(connect_state == MQTT_STATE_CONNECTED){
		printf("has connected !\n");
			return 0;
	}
	if(mqtt_connect(client.mqtt) < 0) {
        printf("mqttc connect failed.\n");
    }
	while(connect_state != MQTT_STATE_CONNECTED)
	{
		time_out++;
		if(time_out > 30){
			printf("connect time out!\n");
			mqtt_disconnect(client.mqtt);
			break;
		}
		sleep(1);
	}
	
	if(connect_state != MQTT_STATE_CONNECTED)
		return -1;
	
	return 0;
}


void mqtt_client::set_sub_callback(subcbFunc cb){
	subcb = cb;
}


bool mqtt_client::HasConnected()
{
	if(connect_state == MQTT_STATE_CONNECTED)
		return true;

	return false;
}
int mqtt_client::disconnect()
{
	int time_out = 0;
	if(connect_state == MQTT_STATE_CONNECTED){
		mqtt_disconnect(client.mqtt);
	}else{
		printf("not connected,no need disconnect !\n");
	}
	return 0;
}


void mqtt_client::subscribe(char* topic,unsigned char qos){
	mqtt_subscribe(client.mqtt, topic, qos);
}

void mqtt_client::publish(char* topic , char* data,int size,uint8_t qos)
{
	MqttMsg * msg = mqtt_msg_new(0, qos, false, false,
				zstrdup(topic), size, data);
	mqtt_publish(client.mqtt, msg);
}

mqtt_client::~mqtt_client(){
	mqtt_disconnect(client.mqtt);
	connect_state = MQTT_STATE_DISCONNECTED;
}

void mqtt_client::sendping()
{
	if(connect_state == MQTT_STATE_CONNECTED)
	{
		if((pinrsp == 0)&&(pinreq == 1))
		{
			printf("mqtt has disconnected !\n");
			mqtt_disconnect(client.mqtt);
			connect_state = MQTT_STATE_DISCONNECTED;
		}else{
			mqtt_ping(client.mqtt);
		}
	}
}


void mqtt_client::on_connect(void *data, int state) {
	_NOTUSED(data);
	Mqtt *mqtt = (Mqtt *)this;// from c 
	mqtt_client *this_client = (mqtt_client *)mqtt->parent;
	switch(state) {
	case MQTT_STATE_CONNECTING:
		printf("sss mqttc is connecting to %s:%d...\n", this_client->client.mqtt->server, this_client->client.mqtt->port);
		this_client->connect_state = MQTT_STATE_CONNECTING;
		break;
	case MQTT_STATE_CONNECTED:
		printf("mqttc is connected.\n");
		this_client->connect_state = MQTT_STATE_CONNECTED;
		break;
	case MQTT_STATE_DISCONNECTED:
		printf("mqttc is disconnected.\n");
		this_client->connect_state = MQTT_STATE_DISCONNECTED;
		break;
	default:
		printf("mqttc is in badstate. %d \n",state);
	}
}


void mqtt_client::on_connack(void *data, int rc) {
	//_NOTUSED(mqtt);
	_NOTUSED(data);
	Mqtt *mqtt = (Mqtt *)this;
	printf("received connack: code=%d\n", rc);
}

void mqtt_client::on_publish(void *data, int msgid) {
	//_NOTUSED(mqtt);
	_NOTUSED(msgid);
	Mqtt *mqtt = (Mqtt *)this;
	MqttMsg *msg = (MqttMsg *)data;
	printf("publish to %s:", msg->topic);
	print_hex(msg->payload, msg->payloadlen);
}
void mqtt_client::on_puback(void *data, int msgid) {
	//_NOTUSED(mqtt);
	_NOTUSED(data);
	printf("received puback: msgid=%d\n", msgid);
}

void mqtt_client::on_pubrec(void *data, int msgid) {
	//_NOTUSED(mqtt);
	_NOTUSED(data);
	printf("received pubrec: msgid=%d\n", msgid);
}

void mqtt_client::on_pubrel(void *data, int msgid) {
	//_NOTUSED(mqtt);
	_NOTUSED(data);
	printf("received pubrel: msgid=%d\n", msgid);
}

void mqtt_client::on_pubcomp(void *data, int msgid) {
	//_NOTUSED(mqtt);
	_NOTUSED(data);
	printf("received pubcomp: msgid=%d\n", msgid);
}
void mqtt_client::on_subscribe(void *data, int msgid) {
	//_NOTUSED(mqtt);
	char *topic = (char *)data;
	printf("subscribe to %s: msgid=%d\n", topic, msgid);
}

void mqtt_client::on_suback(void *data, int msgid) {
	//_NOTUSED(mqtt);
	_NOTUSED(data);
	printf("received suback: msgid=%d\n", msgid);
}
void mqtt_client::on_unsubscribe(void *data, int msgid) {
	//_NOTUSED(mqtt);
	_NOTUSED(data);
	printf("unsubscribe %s: msgid=%d\n", (char *)data, msgid);
}
void mqtt_client::on_unsuback(void *data, int msgid) {
	//_NOTUSED(mqtt);
	_NOTUSED(data);
	printf("received unsuback: msgid=%d\n", msgid);
}

void mqtt_client::on_pingreq(void *data, int id) {
	_NOTUSED(data);
	_NOTUSED(id);
	Mqtt *mqtt = (Mqtt *)this;// from c 
	mqtt_client *this_client = (mqtt_client *)mqtt->parent;	
	this_client->pinreq = 1;
	this_client->pinrsp = 0;
}

void mqtt_client::on_pingresp(void *data, int id) {
	Mqtt *mqtt = (Mqtt *)this;// from c 
	mqtt_client *this_client = (mqtt_client *)mqtt->parent;	
	this_client->pinreq = 0;
	this_client->pinrsp = 1;
	_NOTUSED(data);
	_NOTUSED(id);	
}

void mqtt_client::on_disconnect(void *data, int id) {
	_NOTUSED(data);
	_NOTUSED(id);
	printf("disconnect\n");
}

void mqtt_client::on_message(MqttMsg *msg) {
	Mqtt *mqtt = (Mqtt *)this;// from c 
	mqtt_client *this_client = (mqtt_client *)mqtt->parent;
	printf("received message: topic=%s, payload=%s\n", msg->topic, msg->payload);
	if(!strcmp(msg->topic,"A/AT"))
	{
		uint8_t year = msg->payload[0];
		uint8_t mon = msg->payload[1];
		uint8_t day = msg->payload[2];
		uint8_t hours = msg->payload[3];
		uint8_t min = msg->payload[4];
		uint8_t sec = msg->payload[5];
		printf("sync time! 20%d %d %d %d %d %d \n",year,mon,day,hours,min,sec);
		// date -s "2020-06-11 13:55:00"
		// hwclock -w
		char sync_time_buf[100];
		memset(sync_time_buf,0,100);
		sprintf(sync_time_buf,"date -s \"20%d-%d-%d %d:%d:%d\"",year,mon,day,hours,min,sec);
		system(sync_time_buf);
		system("hwclock -w");
		return;
	}
	if(this_client->subcb != NULL)
		this_client->subcb(msg);
}

void mqtt_client::set_callbacks(Mqtt *mqtt) {
	int i = 0, type;

	MqttCallback callbacks[15] = {
		NULL,
		(MqttCallback)&mqtt_client::on_connect,
		(MqttCallback)&mqtt_client::on_connack,
		(MqttCallback)&mqtt_client::on_publish,
		(MqttCallback)&mqtt_client::on_puback,
		(MqttCallback)&mqtt_client::on_pubrec,
		(MqttCallback)&mqtt_client::on_pubrel,
		(MqttCallback)&mqtt_client::on_pubcomp,
		(MqttCallback)&mqtt_client::on_subscribe,
		(MqttCallback)&mqtt_client::on_suback,
		(MqttCallback)&mqtt_client::on_unsubscribe,
		(MqttCallback)&mqtt_client::on_unsuback,
		(MqttCallback)&mqtt_client::on_pingreq,
		(MqttCallback)&mqtt_client::on_pingresp,
		(MqttCallback)&mqtt_client::on_disconnect
	};
	for(i = 0; i < 15; i++) {
		type = (i << 4) & 0xf0;
		mqtt_set_callback(mqtt, type, callbacks[i]);
	}
	mqtt_set_msg_callback(mqtt, (MqttMsgCallback)&mqtt_client::on_message);
}


void mqtt_client::client_setup(char* server,int port, char* username, char* passwd, int keeplive) {
	char c;
	Mqtt *mqtt = client.mqtt;

	mqtt_set_server(mqtt, server);

	mqtt_set_port(mqtt, port);

	mqtt_set_username(mqtt, username);

	mqtt_set_passwd(mqtt, passwd);

	//mqtt_set_keepalive(mqtt, keeplive);
}

int mqtt_client::client_cron(long long id, void *clientData) {
	aeEventLoop *el = (aeEventLoop *)this;
	Client *client = (Client *)clientData;
	_NOTUSED(el);
	_NOTUSED(id);
    if(client->shutdown_asap) {
		printf("mqttc is shutdown...");
        aeStop(el);
    }
    return 1000;
}

void mqtt_client::mqtt_init(Mqtt *mqtt) {
	mqtt->state = 0;
	mqtt_set_clientid(mqtt, clientid);
	mqtt->port = mhostp;
	mqtt->retries = 3;
	mqtt->error = 0;
	mqtt->msgid = 1;
	mqtt->cleansess = 1;
	mqtt->keepalive = 60;
#if 0
	typedef struct {
	bool retain;
	uint8_t qos;
	const char *topic;
	const char *msg;
} MqttWill;
	MqttWill* will = (MqttWill*)malloc(sizeof(MqttWill));
	will->retain = true;
	will->qos = 2;
	will->topic = "/up/cam/close";
	will->msg = "closed";
	mqtt_set_will(mqtt,will);
#endif
}


void mqtt_client::client_init() {
	aeEventLoop *el;
	el = aeCreateEventLoop();
	client.el = el;
	client.mqtt = mqtt_new(el);
	client.mqtt->parent = (void*)this;
	client.shutdown_asap = false;
	mqtt_init(client.mqtt);
    signal(SIGCHLD, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
    aeCreateTimeEvent(el, 100, (aeTimeProc*)&mqtt_client::client_cron, &client, NULL);
}


