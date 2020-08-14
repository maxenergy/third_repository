#include "mqtt_client.h"
#include "mqtt.h"


/* upstream to publish */
typedef struct __UP_Dev_Status{
    uint8_t cmd_type;   //0
    uint8_t status;     //0:good，1:bad
}Up_Dev_Status;

typedef struct __UP_Recog_Result{
    uint8_t cmd_type;   //08
    uint8_t p_type;     //1-识别成功，2-陌生人
    uint32_t p_id;      //人员ID（陌生人填0占位即可）
    uint16_t year;      //年
    uint8_t month;      //月
    uint8_t day;        //日
    uint8_t hour;       //时
    uint8_t min;        //分钟
    uint8_t sec;        //秒
}Up_Recog_Result;

typedef struct __UP_User_Reg_Result{
    uint8_t cmd_type;   //09
    uint32_t p_id;      //人员ID
    uint8_t result;     //登记结果：1成功， 0失败
}Up_User_Reg_Result;

/* downstream to subscrible */
typedef struct __CTL_Dev{
    uint8_t cmd_type;   //0
    uint8_t status;     //1-重新启动设备
}Ctl_Dev;

typedef struct __CTL_User_Reg{
    uint8_t cmd_type;   //08
    uint32_t p_id;      //人员ID
    uint8_t floor;      //所在楼层
    uint32_t pic_id;    //照片ID
}Ctl_User_Reg;


typedef struct __CTL_User_Del{
    uint8_t cmd_type;   //09
    uint32_t p_id;      //人员ID
}Ctl_User_Del;

typedef struct __CTL_User_Auth{
    uint8_t cmd_type;   //0A
    uint32_t p_id;      //人员ID
    uint8_t perm;       //1-允许通过，2-不允许通过
}Ctl_User_Auth;

typedef struct __CTL_OTA{
	uint8_t cmd_type; 
    uint8_t sw_version;   //08
    uint8_t hw_version;   //08
    uint8_t for_all;
    char ota_devid[20];
	char ota_file_name[20];
}CTL_OTA;


typedef struct __WM_CTL_DEV_REG{
	uint8_t cmd_type; 
    uint8_t sw_version;   //08
    uint8_t hw_version;   //08
    char ota_devid[20];
}WM_CTL_DEV_REG;

//�豸���ӵ������� <-> ���Ͱ汾��Ϣ
//����������ota ����

bool publish_device_status(mqtt_client *mqttc, bool device_status);

bool publish_recognize_result(mqtt_client *mqttc, uint8_t p_type, uint32_t p_id,
                              uint16_t year,uint8_t month,uint8_t day,uint8_t hour, uint8_t min, uint8_t sec);

bool publish_user_register_result(mqtt_client *mqttc, uint32_t p_id, uint8_t result);

bool subscribe_device_control(const char *buffer);

bool subscribe_user_register_or_modify(const char *buffer);

bool subscribe_user_delete(const char *buffer);

bool subscribe_user_auther(const char *buffer);

void userspace_on_message_handler(MqttMsg *msg);
