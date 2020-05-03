#define HTTPSERVER_IMPL
#include "httpserver.h"
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "http-server-lib.h"

#define RESPONSE "OK"
#define RESPONSE_ACK "No support"

http_data_cb http_data_callback = NULL;
image_info_t http_image;

int request_target_is(struct http_request_s* request, char const * target) {
  http_string_t url = http_request_target(request);
  int len = strlen(target);
  return len == url.len && memcmp(url.buf, target, url.len) == 0;
}

struct http_server_s* poll_server;

char *get_file_end_boundary(char *buf, unsigned int len, char *boundary)
{
	char *string_buf = buf;
	unsigned int string_len = 0;
	char *result_string = NULL;	

	while(len > string_len)
	{
		if((result_string = strstr(buf + string_len, boundary)) == NULL)
		{
			string_len += strlen(buf + string_len) + 1;
		}
		else
		{
			break;
		}
	}
	
	return result_string;

}

char *get_boundary(const char *string, bool start)
{
	char *temp_string;
	char *boundary_string;

	if(start && string[0] != '-')
		return NULL;	

	temp_string = strstr(string, "\r\n");
	if(!temp_string)
	{
		printf("body buf not contain CR\r\n");
		return NULL;
	}
	
	if(start && temp_string - string > 60)
	{
		printf("body buf is not formdata\r\n");
		return NULL;
	}

	
	boundary_string = malloc( temp_string - string + 1 );
	memset(boundary_string, 0, temp_string - string + 1);
	memcpy(boundary_string, string, temp_string - string);
	
	return boundary_string;
}

#define MAX_FORMDATA_KEY_NUM 3
typedef enum
{
	FORMDATA_KEY_NAME = 0,
	FORMDATA_KEY_ID,
	FORMDATA_KEY_FILE,
	FORMDATA_KEY_UNDEFINE
}e_formdata_type_t;

int formdata_paser(http_string_t *body)
{
	char *string_buf = body->buf;
	char *temp_string;
	char *name_string;
	char *boundary_string;
	char *field_string;
	int string_len = 0;
	char *name;
	char *ID;
	char *pic_buf;
	unsigned int pic_size;
	int ret = -1;
	
	boundary_string = get_boundary(string_buf, true);
	if(!boundary_string)
	{
		return ret;
	}
	string_len = strlen(boundary_string) + 2;
	
	printf("boundary_string:%s, size:%d \n", boundary_string,  strlen(boundary_string));
	for(int i = 0; i < MAX_FORMDATA_KEY_NUM; i++)
	{
		field_string = get_boundary(string_buf + string_len, false);
		if(!field_string)
			return ret;
		
		string_len += strlen(field_string) + 2;
		printf("field_string:%s, size:%d \n", field_string,  strlen(field_string));
		int formdata_key = FORMDATA_KEY_NAME;
		for(; formdata_key < FORMDATA_KEY_UNDEFINE; formdata_key++)
		{
			switch(formdata_key)
			{
				case FORMDATA_KEY_NAME:
				{
					if(strstr(field_string, "\"Name\"") == NULL)
					{
						continue;
					}
					name = get_boundary(string_buf + string_len + 2, false);
					printf("name:%s\n", name);
					string_len += strlen(name) + 2 + 2;
					http_image.Name = name;
					break;
				}
				case FORMDATA_KEY_ID:
				{
					if(strstr(field_string, "\"ID\"") == NULL)
					{
						continue;
					}
					ID = get_boundary(string_buf + string_len + 2, false);
					printf("ID:%s\n", ID);
					string_len += strlen(ID) + 2 + 2;
					http_image.ID = ID;
					break;
				}
				case FORMDATA_KEY_FILE:
				{
					if(strstr(field_string, "filename") == NULL)
					{
						continue;
					}
					temp_string = get_boundary(string_buf + string_len, false);
					if(!temp_string)
					{
						break;
					}
					printf("file temp_string:%s\n", temp_string);
					string_len += strlen(temp_string) + 2 + 2;
					free(temp_string);
					pic_buf = string_buf + string_len;
					
					temp_string = get_file_end_boundary(pic_buf, body->len - string_len, boundary_string);
					if(!temp_string)
					{
						break;
					}
					printf("string_buf:%p, pic_buf:%p, temp_string :%p, len:%d \n", string_buf, pic_buf, temp_string, string_len);
					pic_size =  temp_string - pic_buf - 2;
					string_len += temp_string - pic_buf;
					printf("picsize = %x\n", pic_size);
					http_image.pic_buf = pic_buf;
					http_image.pic_size = pic_size;
					break;
				}
				default:
					printf("undifined msg\n");
					break;
			
			}
			break;
		}
		free(field_string);
		if(string_len >= body->len)
		{
			return ret;
		}
		temp_string = strstr(string_buf + string_len, boundary_string);
		if(temp_string == string_buf + string_len)
		{
			if(temp_string[strlen(boundary_string) + 1 ] == '-')
			{
				ret = 0;
				break;
			}
			else
			{
				string_len += strlen(boundary_string) + 2;
			}
		}
	}
	free(boundary_string);
	
	return ret;
}

void handle_request(struct http_request_s* request) {
  http_request_connection(request, HTTP_AUTOMATIC);
  struct http_response_s* response = http_response_init();
  http_response_status(response, 200);
  if (request_target_is(request, "/image")) {
    http_string_t body = http_request_body(request);
    if(formdata_paser(&body))
	printf("err msg\n"); 
    else
    {
	if(http_data_callback)
	{
		http_message_t t_http_message;
		t_http_message.nCmd = CMD_IAMGE;
		t_http_message.nMessage = &http_image;
		http_data_callback(&t_http_message);
		free(http_image.Name);
		free(http_image.ID);
	}
    }
    http_response_header(response, "Content-Type", "text/plain");
    http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
  }
  else
  {
     http_response_header(response, "Content-Type", "text/plain");
     http_response_body(response, RESPONSE_ACK, sizeof(RESPONSE_ACK) - 1);
  }
  http_respond(request, response);
}

struct http_server_s* server;

int *http_server_thread(void)
{
  	server = http_server_init(8080, handle_request);
 	http_server_listen(server);

	return NULL;
}

int http_server_main(void) 
{
   	pthread_t http_thread_id;

	if ((pthread_create(&http_thread_id, NULL, http_server_thread, NULL)) == -1)
	{
		printf("create http server thread error!\n");
		return 1;
	}
	return 0;
}

int http_server_regist_callback(http_data_cb func)
{
	http_data_callback = func;

	return 0;
}
