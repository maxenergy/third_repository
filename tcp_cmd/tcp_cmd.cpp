#include "tcp_cmd.h"

Tcp_Cmd::Tcp_Cmd(int port,UpFaceFunc cb_func,OTAfunc otacb){
    port_id = port;
	connect_state = 0;
	start_flag = 1;
	multi_pkt_buf = NULL;
	wdata_len = 0;
	upface_func = cb_func;
	ota_func = otacb;
	G_cmd_buf_index = 0;
	connect_loop=std::thread(&Tcp_Cmd::connect_loop_f,this);
	connect_loop.detach();

	recv_loop=std::thread(&Tcp_Cmd::recv_loop_f,this);
	recv_loop.detach();
}

Tcp_Cmd::~Tcp_Cmd(){
	start_flag = 0;
}

int Tcp_Cmd::bind_server(int server_s, const char *server_ip, uint16_t server_port)
{
    struct sockaddr_in server_sockaddr;

    memset(&server_sockaddr, 0, sizeof server_sockaddr);
    server_sockaddr.sin_family = AF_INET;
    if (server_ip)
        inet_aton(server_ip, &server_sockaddr.sin_addr);
    else
        server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    server_sockaddr.sin_port = htons(server_port);

    return bind(server_s, (struct sockaddr *)&server_sockaddr,
                sizeof(server_sockaddr));
}

int Tcp_Cmd::create_tcp_server_socket(const char *server_ip, uint16_t server_port)
{
    int server_s;
    int ret;
    int sock_opt = 1;
	
    server_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (server_s == -1) {
        printf("unable to create socket");
    }
    /* reuse socket addr */
    if ((setsockopt(server_s, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,
                    sizeof(sock_opt))) == -1) {
        printf("setsockopt");
    }

    /* internet family-specific code encapsulated in bind_server()  */
    if (bind_server(server_s, server_ip, server_port) == -1) {
        printf("unable to bind");
    }

    /* listen: large number just in case your kernel is nicely tweaked */
    if (listen(server_s, SO_MAXCONN) == -1) {
        printf("unable to listen");
    }
    return server_s;
}



 void Tcp_Cmd::connect_loop_f(){
	struct sockaddr_in remote_addr;
	socklen_t remote_addrlen = sizeof(struct sockaddr_in);
	printf("create_tcp_server_socket ++ !\n");
	sock_fd = create_tcp_server_socket(NULL, port_id);
	printf("create_tcp_server_socket --!\n");
	
	while(start_flag){
		usleep(1000*50);
		new_fd = accept(sock_fd, (struct sockaddr *)&remote_addr,
			 &remote_addrlen);
		if(new_fd < 0){
			printf("accept fail!");
		}else{
		    printf("connect ok!");
			connect_state = 1;
		}
  }
}

 void Tcp_Cmd::send_rsp(char* data,int len)
 {
 	int ret = 0;
	if ((ret = send(new_fd, data,len, 0)) < 0) {
		printf("----- send fail errno ----\n");
	}
 }

RSP_TYPE Tcp_Cmd::process_single_pkt(uint8_t cmd_type,uint8_t * buf, int len)
{
  	printf("get single pkt!\n");
	deliver_cmd(cmd_type,buf,len);
	return RSP_OK;
}

void Tcp_Cmd::deliver_cmd(uint8_t cmd_type,uint8_t *buf,int len)
{
	uint8_t *cmd_buf;
	printf("deliver_cmd \n");
	switch(cmd_type) {
		case UPSTREAM_FACE:
			 cmd_buf = (uint8_t *)malloc(len);
			 memcpy((char *)cmd_buf,(char *)buf,len);
			 cmd_buf[19] = 0; //the end of name,must be 0.
			 upface_func((Face_Upstream_First*)cmd_buf);			 
			 free(cmd_buf);
			 break;
	    case REMOVE_FACE:
			 printf("REMOVE_FACE !\n");
			 break;
	    case DEVICE_UPDATE:
			 cmd_buf = (uint8_t *)malloc(len+1);
			 memcpy((char *)cmd_buf,(char *)buf,len);
			 cmd_buf[len] = 0x0;
			 ota_func((char*)cmd_buf);
			 free(cmd_buf);
			 break;
		default:
			printf("skip unkonw cmd !\n");
	}
}

 RSP_TYPE Tcp_Cmd::check_pkt(Tcp_Message_t *msg)
 {
	if(ntohl(msg->total_size) != mpkt_total_size)
		return RSP_ERR;
	if((mpkt_write_index + ntohl(msg->cur_size)) > mpkt_total_size)
		return RSP_ERR;
	if(multi_pkt_buf == NULL)
		return RSP_ERR;

	return RSP_OK;
 }

 RSP_TYPE Tcp_Cmd::process_multi_pkt(Tcp_Message_t *msg,uint8_t* data_buf)
 {
 	 int temp_cur_size = ntohl(msg->cur_size); 
	 printf("get multi pkt type %d cur size %d  total size %d !\n",msg->pkt_type,temp_cur_size,ntohl(msg->total_size));
	 if(msg->pkt_type == MUTIL_PKT_START)
	 {
		if(multi_pkt_buf != NULL){
			free(multi_pkt_buf);
			multi_pkt_buf = NULL;
			mpkt_write_index =0;
		}
		mpkt_total_size = ntohl(msg->total_size);
		multi_pkt_buf = (uint8_t *)malloc(mpkt_total_size);

		if(check_pkt(msg) != RSP_OK)
		{
			free(multi_pkt_buf);
			multi_pkt_buf = NULL;
			mpkt_write_index =0;
			return RSP_ERR;
		}
		
		memcpy(multi_pkt_buf+mpkt_write_index,data_buf,temp_cur_size);
		mpkt_write_index += temp_cur_size;
	 }else if(msg->pkt_type == MUTIL_PKT_END)
	 {
		 if(check_pkt(msg) != RSP_OK)
		 {
			 return RSP_ERR;
		 }
		memcpy(multi_pkt_buf+mpkt_write_index,data_buf,temp_cur_size);
		printf("mpkt_write_index %d temp_cur_size %d mpkt_total_size %d \n",mpkt_write_index,temp_cur_size,mpkt_total_size);
		deliver_cmd(msg->cmd_type,multi_pkt_buf,mpkt_total_size);
	 }else{
		 if(check_pkt(msg) != RSP_OK)
		 {
			 return RSP_ERR;
		 }
        memcpy(multi_pkt_buf+mpkt_write_index,data_buf,temp_cur_size);
		mpkt_write_index += temp_cur_size;
	 }
	  return RSP_OK;
 }

int Tcp_Cmd::find_msg(uint8_t* data,int len)
{
	int i =0;
	for(i =0;i<len-sizeof(Tcp_Message_t);i++)
	{
		Tcp_Message_t *msg = (Tcp_Message_t *)(data+i);
		if((msg->pkt_head == SEND_CMD)&&(msg->magicnum == 0x13))
			return i;
	}
	return -1;
}


void Tcp_Cmd::bufferred_cmd(uint8_t* data,int len)
{
	Rsp_Head rsp;	
	int msg_index = 0;
	int last_size =0;
	memcpy(G_cmd_buffer+G_cmd_buf_index,data,len);
	G_cmd_buf_index += len;

	while(last_size>=0){
		msg_index = find_msg(G_cmd_buffer,G_cmd_buf_index);

		if(msg_index < 0)
		{
			printf("get a error!\n");
			return;
		}else if(msg_index >0)
		{
			printf("lost a pkt!!!!!!!!!!!!! %d \n",msg_index);
		}
		
		Tcp_Message_t *msg = (Tcp_Message_t *)(G_cmd_buffer + msg_index);
		int buf_len = ntohl(msg->cur_size);
		last_size = G_cmd_buf_index - (msg_index+buf_len+sizeof(Tcp_Message_t));
		#if 0
		printf("last_size %d =  G_cmd_buf_index %d - (%d + %d + %d )",
			last_size,\
			G_cmd_buf_index,\
			msg_index,\
			buf_len,\
			sizeof(Tcp_Message_t)/sizeof(char));
		#endif
		if(last_size >= 0){
			process_data((G_cmd_buffer + msg_index),buf_len + sizeof(Tcp_Message_t));
			memcpy(G_cmd_buffer,G_cmd_buffer+buf_len + sizeof(Tcp_Message_t) + msg_index,last_size);
			G_cmd_buf_index = last_size;
			printf("G_cmd_buf_index is %d msg_index is %d \n",G_cmd_buf_index,msg_index);
		}
	}
}
 void Tcp_Cmd::process_data(uint8_t* data,int len)
{
	Rsp_Head rsp;
	int ret = 0;
	Tcp_Message_t *msg = (Tcp_Message_t *)data;
	if((msg->pkt_head == SEND_CMD)&&(msg->magicnum == 0x13)){
		if(msg->pkt_type== SINGLE_PKT){
			ret = process_single_pkt(msg->cmd_type, data + sizeof(Tcp_Message_t),ntohl(msg->cur_size));
		}else if((msg->pkt_type > SINGLE_PKT)&&(msg->pkt_type <= MUTIL_PKT_END))
		{
			ret = process_multi_pkt(msg, data + sizeof(Tcp_Message_t));
		}
		rsp.cmd_type = msg->cmd_type;
		rsp.magicnum = 0x13;
		rsp.pkt_head = SEND_RSP;
		rsp.seq_num = msg->seq_num;
		rsp.ret = ret;
	}
	
}

#define MAX_BUF_SIZE 900

 void Tcp_Cmd::recv_loop_f(){
	char buf[MAX_BUF_SIZE];
	int ret = 0;
	while(start_flag)
	{
		if(connect_state)
		{
			ret = recv(new_fd, buf, MAX_BUF_SIZE, 0);
		        if (ret < 0){
		            printf("recv fail \n");
		        } else if (!ret) {
		            printf("client have closed \n");
					connect_state = 0;
		        }else if(ret > 0){
					bufferred_cmd((uint8_t*)buf,ret);
		        }
		}else{
			usleep(1000*100);
		}
	}
}
