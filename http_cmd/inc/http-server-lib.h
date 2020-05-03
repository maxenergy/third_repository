#ifndef __HTTP_SERVER_LIB__
#define __HTTP_SERVER_LIB__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	CMD_IAMGE = 0,
	CMD_CONTROL,
	CMD_MAX,
}message_cmd_t;

typedef struct http_message
{
	message_cmd_t nCmd;
	void *nMessage;
}http_message_t;

typedef struct image_info
{
	char *Name;
	char *ID;
	unsigned char *pic_buf;
	unsigned int pic_size;
}image_info_t;	

typedef int (*http_data_cb) (http_message_t *);

int http_server_main(void) ;
int http_server_regist_callback(http_data_cb func);

#ifdef __cplusplus
}
#endif

#endif
