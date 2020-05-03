#ifndef __HTTPSERVERLIB__H__
#define __HTTPSERVERLIB__H__

#include "http-server-lib.h"

class HttpServerLibApi
{
	public:
		HttpServerLibApi();
	int StartHttpServer(void);
	int SetHttpServerCallback(http_data_cb func);
};

#endif
