/*
 *	http.h
 *
 *	HTTP facilities
 */
#ifndef IO_HTTP_H
#define IO_HTTP_H
#include <stdint.h>
#include <string.h>

/*
	"aux" is a reserved device name on Windows and causes both git-bash and CMake to error out when accessing the file name.
	I've changed the file names to _aux.h and _aux.c to fix it. My apologies to the original author of OAuth.  --Dennis Ferron
*/
#include "_aux.h"

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


int http_request(struct url_props *url, const char *method, const struct string *headers, const struct string *content, struct http_response *resp);
int parse_url(const char *url, struct url_props *d);
void http_response_dealloc(struct http_response *self);
void http_response_init(struct http_response *self);
#endif
