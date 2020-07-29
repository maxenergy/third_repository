#include "mainwindow.h"
#include "facerecognition_public.h"
#include <QApplication>
#include <QDebug>
#include <QWSServer>

#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <dlfcn.h>


#include "include/main.h"
//#include "include/shared_quene.h"

using namespace std;
extern "C" {
extern int sysinit(int *fd_out,int mroute,int mflip);
#ifndef BUILD_FACTORY_TEST_APP
extern void log_level_set(int level);
extern void init_pola_sdk(int liveless_mode,float detect_threshold,int test_flag);
#endif
}

int camroute = 90;
int camflip = 1;

int param_check(const char* str)
{
        if(sizeof(str) > MAX_PARAM_SIZE)
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
        memset(fixed_dev_sn,0,20);
        sprintf(fixed_dev_sn,"%s_%x%x%x%x%x%x_%d",CUSTOM_ID,hw_addr[0],hw_addr[1], \
                hw_addr[2],hw_addr[3],hw_addr[4],hw_addr[5],DEV_TYPE);

        printf("fixed_dev_sn %s \n",fixed_dev_sn);
        //close(sock);
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
                                default:
                                        printf("error param! \n");
                                        break;
                        }
                }
        }
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

int get_irled_state()
{
        int state = 0;
        state =  gpio_read(GPIO_IR_LED_IN);
        if(state == 0)
                return 1;
        else
                return 0;
}

int save_DevConfig(const char *config_buffer)
{
    int fd;
    int tmp_value = 0;
    size_t size;
    char QRdata[1024] = {0};
    char dev_config_data[1024] = {0};
    bool is_success;

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

        /* If Security_level is 1, only IP address and Camera Route,Flip could be set. */
        if ((security_level == 1)&&((Key!='I')||(Key!='J')||(Key!='K')||(Key!='L')||(Key!='M')))
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

            default:
                printf("ERROR: this key %s key is doesn't be supported now.\n", temp);
                return -1;
        }
    }

    for(int p=0; p <= 10; p++)
    {
        set_ircut(1);
        sleep(1);
        set_ircut(0);
    }

    printf("device has been configed,need reboot!\n");
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

void thread_qrcode_setup()
{
        bool scan_ret = false;
        printf("qrcode thread started!\n");
        while(1)
        {
                //sleep(1);
                usleep(1000 * 50);
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

int aiot_add_face(char* name ,int id,int floor,char* filename)
{
        aiot_server->ftpdown(filename);
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

void save_pic(char* name)
{
        VIFrame mBtnPhoto;
        char full_name[255];
        //unsigned char* rgb_buf;
        FaceRecognitionApi::getInstance().capture(OTA_PIC_CHN,mBtnPhoto);
//      struct timeval start_time;
//      struct timeval stop_time;

        cv::Mat yuvFrame = cv::Mat(mBtnPhoto.mHeiht*3/2, mBtnPhoto.mWidth, CV_8UC1, mBtnPhoto.mData);
        cv::Mat dstImage;
//       gettimeofday(&start_time, NULL);
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

#define MQTT_PING_TIME_PRESEC  50

void thread_aiot_setup()
{
        int ret =0;
        ret = aiot_server->mqttc_init(mqtt_server_ip,mqtt_user, mqtt_passwd, mqtt_server_port, mqtt_user);
        if(ret != MQTTC_CONNECTED){
                printf("Mqttc connect failed!\n");
        }
        aiot_server->ftp_init(ftp_serverip, ftp_user, ftp_passwd, ftp_server_port);
        while(1){
                sleep(MQTT_PING_TIME_PRESEC);
                if(aiot_server->mqttc->HasConnected())
                {
                        aiot_server->mqttc->sendping();
                }else{
                        printf("zq mattc is disconnect,try to reconnect!\n");
                        aiot_server->mqttc_reconnect();
                }
                print_timeinfo();
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
                                aiot_server->ftp_up_trigger(sf->file_name);
                        }
                        free(sf);
                }
        }
}

int main(int argc, char *argv[])
{
    int fd =0;
    std::cout << "pola sdk init!!! " << std::endl;
    sysinit(&fd,camroute,camflip);
#ifndef BUILD_FACTORY_TEST_APP
    log_level_set(1);
    init_pola_sdk(0,0.4,0);
#endif

    /* Initial Device */
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

    aiot_server = new cloud_server(mqtt_uptopic,mqtt_downtopic,ota_mqtt_downtopic,sw_ver,hw_ver);
    aiot_server->set_cmd_cb([&](down_event event){
        return process_aiotevent(event);
    });

    save_file_loop=std::thread(&thread_save_file);
    save_file_loop.detach();

    aiot_setup_once = std::thread(&thread_aiot_setup);
    aiot_setup_once.detach();

    ir_cut_thread = std::thread(&thread_ircut);
    ir_cut_thread.detach();

    qrcode_loop = std::thread(&thread_qrcode_setup);
    qrcode_loop.detach();


    FaceRecognitionApi &app = FaceRecognitionApi::getInstance();
    if (!app.init()) {
        qDebug() << "app init failed";
        return 1;
    };
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
