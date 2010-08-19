#ifndef IO_HTTP_H
#define IO_HTTP_H
#include <stdint.h>
#include <string.h>
#include "aux.h"

struct url_props
{
	const char *hostName;
	size_t hostNameLen;
	const char *path;
	size_t pathLen;
	int port;
};

struct http_response
{
	char *body;
	size_t bodySize;
	size_t bodyLen;

	int headersSize;
	int contentLen;
	uint32_t respCode;
};


int HttpRequest(struct url_props *url, const char *method, const struct string *headers, const struct string *content, struct http_response *resp);
int ParseUrl(const char *url, struct url_props *d);
void EnsureBodyCapacity(struct http_response *resp, const size_t n);
void ReleaseHttpResponse(struct http_response *resp);
#endif
