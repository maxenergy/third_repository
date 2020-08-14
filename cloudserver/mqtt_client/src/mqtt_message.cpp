#include <string.h>

#include "mqtt_message.h"

extern void print_hex(const char *buffer, int size);

/* functions */
bool publish_device_status(mqtt_client *mqttc, bool device_status)
{
	char *publish_buffer = (char *)malloc(2);
	*publish_buffer = 0x0;
	*(publish_buffer+1) = device_status;

	mqttc->publish("UP/CAM/ZQ", publish_buffer, 2, 1);
	return true;
}

bool publish_recognize_result(mqtt_client *mqttc, uint8_t p_type, uint32_t p_id,
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

bool publish_user_register_result(mqtt_client *mqttc, uint32_t p_id, uint8_t result)
{
	char cmd_type = 0x9;
	uint32_t p_id_net = htonl(p_id);
	char *publish_buffer = (char *)malloc(6);
	memcpy(publish_buffer, &cmd_type, 1);
	memcpy(publish_buffer+1, &p_id_net, 4);
	memcpy(publish_buffer+5, &result, 1);

	mqttc->publish("UP/CAM/ZQ", publish_buffer, 6, 1);
	return true;
}

bool subscribe_device_control(const char *buffer)
{
	Ctl_Dev *ctl = (Ctl_Dev *)buffer;
	printf("device_contol:Status- %d", ctl->status);
	return ctl->status;
}

bool subscribe_user_register_or_modify(const char *buffer)
{
	Ctl_User_Reg user;
	memcpy(&(user.p_id), buffer+1, 4);
	memcpy(&(user.floor), buffer+5, 1);
	memcpy(&(user.pic_id), buffer+6, 4);

	printf("user_register:ID- %d, Floor- %d, PIC_ID- %d\n", ntohl(user.p_id), user.floor, ntohl(user.pic_id));

	return true;
}

bool subscribe_user_delete(const char *buffer)
{
        Ctl_User_Del user;
	memcpy(&(user.p_id),buffer+1,4);

        printf("user_delete: ID- %d\n", ntohl(user.p_id));

        return true;
}

bool subscribe_user_auther(const char *buffer)
{
        Ctl_User_Auth user;
	memcpy(&(user.p_id),buffer+1,4);
	memcpy(&(user.perm),buffer+5,1);

        printf("user_auther: ID- %d, permit- %d\n", ntohl(user.p_id), user.perm);

        return true;
}


void userspace_on_message_handler(MqttMsg *msg)
{
	uint8_t cmd_type = *((char *)(msg->payload));
	printf("cmd_type: %d\n", cmd_type);
	switch(cmd_type)
	{
		case 0x00:
			subscribe_device_control(msg->payload);
			break;
		case 0x08:
			subscribe_user_register_or_modify(msg->payload);
			break;
		case 0x09:
			subscribe_user_delete(msg->payload);
			break;
		case 0x0a:
			subscribe_user_auther(msg->payload);
			break;
		default:
			printf("ERROR: cmd_type not recognized.\n");
			break;
	}
}
