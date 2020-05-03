#include "http-server-lib.h"
#include "HttpServerlib.h"

class HttpServerLibApi;

HttpServerLibApi::HttpServerLibApi()
{
}

int HttpServerLibApi::StartHttpServer(void)
{
	return http_server_main();
}

int HttpServerLibApi::SetHttpServerCallback(http_data_cb func)
{
	return http_server_regist_callback(func);
}
