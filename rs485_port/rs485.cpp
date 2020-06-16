#include "rs485.h"
rs485_port::rs485_port(int port_id,int Baudrate){
    int ret =0;
	start_flag = 0;
	port_fd  = open("/dev/ttyAMA1", O_RDWR|O_NOCTTY|O_NDELAY);
	if(port_fd < 0){
		printf("rs485_port open failed %d !\n",port_fd);
	}else{
		ret = setup(9600,0,8,1,'N');
		if(ret == 0){
			printf("rs485 port is ok!\n");
			start_flag = 1;
		}
	}
	cmd_loop=std::thread(&rs485_port::process_cmddata,this);
	cmd_loop.detach();
}

int rs485_port::setup(int speed,int flow_ctrl,int databits,int stopbits,int parity)
{  
    int   i;  
    int   status;  
    int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};  
    int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};  

    system("himm 0x111f0000 0x430");
	system("himm 0x111f0004 0x430");
	//./himm 0x111f0000 0x430./himm 0x111f0004 0x430
    system("echo 40 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio40/direction");
    struct termios options;
    if( tcgetattr(port_fd,&options)  !=  0)  
    {  
        printf("SetupSerial 1");      
        return(-1);   
    }  
    
    //���ô������벨���ʺ����������  
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
    {  
        if  (speed == name_arr[i])  
        {               
            cfsetispeed(&options, speed_arr[i]);   
            cfsetospeed(&options, speed_arr[i]);    
        }  
    }       
     
    //�޸Ŀ���ģʽ����֤���򲻻�ռ�ô���  
    options.c_cflag |= CLOCAL;  
    //�޸Ŀ���ģʽ��ʹ���ܹ��Ӵ����ж�ȡ��������  
    options.c_cflag |= CREAD;  
    
    //��������������  
    switch(flow_ctrl)  
    {  
        case 0 ://��ʹ��������  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
        case 1 ://ʹ��Ӳ��������  
              options.c_cflag |= CRTSCTS;  
              break;  
        case 2 ://ʹ�����������  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //��������λ  
    //����������־λ  
    options.c_cflag &= ~CSIZE;  
    switch (databits)  
    {    
        case 5    :  
                     options.c_cflag |= CS5;  
                     break;  
        case 6    :  
                     options.c_cflag |= CS6;  
                     break;  
        case 7    :      
                 options.c_cflag |= CS7;  
                 break;  
        case 8:      
                 options.c_cflag |= CS8;  
                 break;    
        default:     
                 printf("Unsupported data size\n");  
                 return (-1);   
    }  
    //����У��λ  
    switch (parity)  
    {    
        case 'n':  
        case 'N': //����żУ��λ��  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~INPCK;      
                 break;   
        case 'o':    
        case 'O'://����Ϊ��У��      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   
        case 'e':   
        case 'E'://����ΪżУ��    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  
        case 's':  
        case 'S': //����Ϊ�ո�   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    
             printf("Unsupported parity\n");
             return (-1);   
    }   
    // ����ֹͣλ   
    switch (stopbits)  
    {    
        case 1:     
                 options.c_cflag &= ~CSTOPB; break;   
        case 2:     
                 options.c_cflag |= CSTOPB; break;  
        default:     
           printf("Unsupported stop bits\n");   
           return (-1);  
    }  
     
    //�޸����ģʽ��ԭʼ�������  
    options.c_oflag &= ~OPOST;  
    
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
    //options.c_lflag &= ~(ISIG | ICANON);  
     
    //���õȴ�ʱ�����С�����ַ�  
    options.c_cc[VTIME] = 1; /* ��ȡһ���ַ��ȴ�1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* ��ȡ�ַ������ٸ���Ϊ1 */  
     
    //�����������������������ݣ����ǲ��ٶ�ȡ ˢ���յ������ݵ��ǲ���  
    tcflush(port_fd,TCIFLUSH);  
     
    //�������� (���޸ĺ��termios�������õ������У�  
    if (tcsetattr(port_fd,TCSANOW,&options) != 0)    
    {  
        printf("com set error!\n");    
        return (-1);   
    }  
    return (0);   
}  

 char rs485_port::calc_ecc(Rs485_Cmd *cmd)
{
	char out = 0;
	char *data = (char*)cmd;
	for(int i =0;i<7;i++)
	{
		out += data[i];
	}
	return ~out;
}

int rs485_port::ctl_sw(uint8_t board_id,int sw_id,int onoff){
	Rs485_Cmd *cmd = (Rs485_Cmd *)malloc(sizeof(Rs485_Cmd)); 
	memset(cmd,0,sizeof(Rs485_Cmd));
	cmd->start_byte = START_TYPE_SNED;
	cmd->cmd = CMD_TYPE_SW;
	cmd->addr = board_id;
	cmd->ctl_sw[3-sw_id/8] = onoff<<(sw_id%8);
    cmd->ecc = calc_ecc(cmd);
	std::unique_lock<std::mutex> locker(mMutex);
	if(start_flag)
	cmd_fifo.push_back(cmd);
}

int rs485_port::ctl_relay(uint8_t board_id,int onoff){
	Rs485_Cmd *cmd = (Rs485_Cmd *)malloc(sizeof(Rs485_Cmd)); 
	memset(cmd,0,sizeof(Rs485_Cmd));
	cmd->start_byte = START_TYPE_SNED;
	cmd->cmd = CMD_TYPE_RELAY;
	cmd->addr = board_id;
	cmd->ctl_sw[3] = onoff;
	cmd->ecc = calc_ecc(cmd);
	std::unique_lock<std::mutex> locker(mMutex);
	if(start_flag)
	cmd_fifo.push_back(cmd);
}


void rs485_port::process_cmddata()
{
    int event_size;
	int rsp_status =0;
	while(start_flag)
	{
		usleep(50*1000);
		event_size = cmd_fifo.size();
		if(event_size > 0){
			Rs485_Cmd *item =NULL;
			{
				std::unique_lock<std::mutex> locker(mMutex);
				item = cmd_fifo.front();
			}
			if(item != NULL){
				rsp_status = 0;
				Rs485_Cmd rdata;
				int retlen = 0;
				system("echo 1 > /sys/class/gpio/gpio40/value");
				write(port_fd,item,sizeof(Rs485_Cmd));
				printf("write data is %x %x %x %x %x %x %x %x\n",item->start_byte,item->addr,item->cmd,item->ctl_sw[0], \
				item->ctl_sw[1],item->ctl_sw[2],item->ctl_sw[3],item->ecc);
				system("echo 0 > /sys/class/gpio/gpio40/value");
				
				retlen = read(port_fd,&rdata,sizeof(Rs485_Cmd));
				if(retlen != sizeof(Rs485_Cmd)){
					printf("rsp error %d!\n",retlen);
					rsp_status = 0;
					if(retlen>0)
					{
						printf("read data is %x %x %x %x %x %x %x %x\n",rdata.start_byte,rdata.addr,rdata.cmd,rdata.ctl_sw[0], \
				rdata.ctl_sw[1],rdata.ctl_sw[2],rdata.ctl_sw[3],rdata.ecc);
					}
				}else{
					if((rdata.start_byte == START_TYPE_RECV)&&(rdata.ecc == calc_ecc(&rdata)))
					{
						printf("send cmd  ok!\n");
						rsp_status = 0;
					}else{
						rsp_status = -1;
					}
				}
				if(rsp_status == 0)
				{
					std::unique_lock<std::mutex> locker(mMutex);
					cmd_fifo.pop_front();
					free(item);
				}
				
		   }
		}
	}
}
