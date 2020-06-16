#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<termios.h> 
#include<time.h>
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include<sys/types.h>
#include<sys/stat.h>

#include <vector>
#include <string>
#include <array>
#include <list>
#include <mutex>
#include <thread>
#include <string.h>


#define START_TYPE_SNED   0XA5
#define START_TYPE_RECV   0X5A
#define CMD_TYPE_RELAY    0x01
#define CMD_TYPE_SW    0x20


typedef struct _Rs485_Cmd{
	unsigned char start_byte;
	unsigned char addr;
	unsigned char cmd;
	unsigned char ctl_sw[4];
	unsigned char ecc;
}Rs485_Cmd;

class rs485_port{
public:
	rs485_port(int port_id,int Baudrate);
	~rs485_port();
	int ctl_sw(uint8_t board_id,int sw_id,int onoff);
	int ctl_relay(uint8_t board_id,int onoff);
	int setup(int speed,int flow_ctrl,int databits,int stopbits,int parity);
	char calc_ecc(Rs485_Cmd *cmd);
	void process_cmddata();  
	std::list<Rs485_Cmd*> cmd_fifo;
	std::thread cmd_loop;
    int port_fd;
	std::mutex mMutex;
	int start_flag;
};
