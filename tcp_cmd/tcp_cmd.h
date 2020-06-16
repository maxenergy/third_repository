#ifndef _TCP_CMD_H_
#define _TCP_CMD_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <string.h>
#include <thread>

//cmd_type
enum CMD_TYPE{
	UPSTREAM_FACE = 0,
	REMOVE_FACE,
	DEVICE_UPDATE,
	MAX_CMD_TYPE_SIZE,
};
// single
enum TCP_PKT_TYPE{
	SINGLE_PKT = 0,
	MUTIL_PKT_START,
	MUTIL_PKT_MID,
	MUTIL_PKT_END,
	MAX_PKT_TYPE_SIZE,
};

enum PKT_HEAD{
	SEND_CMD = 0X77,
	SEND_RSP = 0X55
};

#define SO_MAXCONN              10
typedef struct __Face_Upstream_First{
	uint8_t  name[20];
	unsigned int  userid;
	unsigned int image_width;
	unsigned int image_height;
}Face_Upstream_First;



typedef struct __Message_Head{
    uint8_t profile;
	uint8_t magicnum;
	uint8_t cmd_type;
	uint8_t seq_num;
	unsigned int total_size;
	unsigned int  pkt_size;
	uint8_t data_ecc;
}Message_Head;

#define RECV_OK   0x80
#define RECV_ERR  0x33

typedef struct __Rsp_Head {
    uint8_t pkt_head; // 0x77
	uint8_t magicnum;
	uint8_t cmd_type;
	uint8_t seq_num;
	uint8_t ret;
}Rsp_Head;

enum RSP_TYPE{
	RSP_OK = 0,
	RSP_ERR,
};



typedef struct __Tcp_Message_t {
	uint8_t pkt_head; // 0x77
	uint8_t magicnum; // 0x13
	uint8_t pkt_type; //
	uint8_t seq_num; //only for multi pkt
	uint8_t cmd_type; //
	unsigned int total_size; //总包数据长度
	unsigned int cur_size; //当前包数据长度
}Tcp_Message_t;


class Tcp_Cmd
{
public:
		typedef std::function<int(Face_Upstream_First*)> UpFaceFunc;
		typedef std::function<int(char*)> OTAfunc;
		Tcp_Cmd(int port,UpFaceFunc cb_func,OTAfunc otacb);
	    ~Tcp_Cmd();
		int init(int port);
		int stop(void);
		int bind_server(int server_s, const char *server_ip, uint16_t server_port);
		int create_tcp_server_socket(const char *server_ip, uint16_t server_port);
		void connect_loop_f();
		void recv_loop_f();
		void process_data(uint8_t* data,int len);
		void send_rsp(char* data,int len);
		void deliver_cmd(uint8_t cmd_type,uint8_t *buf,int len);
		RSP_TYPE check_pkt(Tcp_Message_t *msg);
		RSP_TYPE process_multi_pkt(Tcp_Message_t *msg,uint8_t* data_buf);
		RSP_TYPE process_single_pkt(uint8_t cmd_type,uint8_t * buf, int len);
		void bufferred_cmd(uint8_t* data,int len);
		int find_msg(uint8_t* data,int len);
   		std::thread recv_loop;
		std::thread connect_loop;
		int port_id;
		int start_flag;
		int connect_state;
		int sock_fd;
		int new_fd;
		int wdata_len;
		uint8_t *multi_pkt_buf;
		int mpkt_write_index;
		int mpkt_total_size;
		UpFaceFunc upface_func;
		OTAfunc ota_func;
		uint8_t G_cmd_buffer[3000];
		int G_cmd_buf_index;
};
#endif
