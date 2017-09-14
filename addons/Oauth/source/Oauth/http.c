/*
 * 	http.c
 *
 *	HTTP facilities
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <strings.h>
#include <ctype.h>
#include "http.h"
#include "tls.h"

/*
	"aux" is a reserved device name on Windows and causes both git-bash and CMake to error out when accessing the file name.
	I've changed the file names to _aux.h and _aux.c to fix it. My apologies to the original author of OAuth.  --Dennis Ferron
*/
#include "_aux.h"




void http_response_dealloc(struct http_response *self)
{	
	if (self->body)
	{
		free(self->body);
		self->body = NULL;
	}
	self->bodySize= 0;
	self->bodyLen = 0;
}

void http_response_init(struct http_response *self)
{
	memset(self, 0, sizeof(struct http_response));
}

static void http_response_ensurecapacity(struct http_response *self, const size_t n)
{
	const size_t capacity = self->bodySize - self->bodyLen;

	if (capacity < n)
	{
		self->bodySize += n - capacity;
		self->body 	= (char *)realloc((void *)self->body, sizeof(char) * self->bodySize);
	}
}

static inline int NeedUrlEncoding(const char c)
{
	switch (c)
	{
		case '#':
		case '0' ... '9':
		case 'A' ... 'Z':
		case 'a' ... 'z':
		case '-':
		case '_':
			return 0;

		default:
			return 1;
	}
}


int parse_url(const char *url, struct url_props *d)
{
	if (strncasecmp(url, "http://", 7) == 0)
	{
		d->port = 80;
		url+=7;
	}
	else if (strncasecmp(url, "https://", 8) == 0)
	{
		d->port = 443;
		url+=8;
	}
	else
		return -1;


	for (d->hostName = url; ;++url)
	{
		if (*url == ':')
		{
			d->hostNameLen = url - d->hostName;
			for (++url, d->port = 0; isdigit(*url); ++url)
				d->port = d->port * 10 + (*url - '0');
			break;
		}
		else if (*url == '/')
		{
			d->hostNameLen = url - d->hostName;
			break;
		}
		else if (*url == '\0')
		{
			d->hostNameLen = url - d->hostName;
			break;
		}
	}

	if (d->hostNameLen < 2 || d->hostNameLen > 255)
		return -1;


	for (d->path = url; *url;++url)
		continue;

	d->pathLen = url - d->path;
	return 0;
}


int http_request(struct url_props *url, const char *method, const struct string *headers, const struct string *content, struct http_response *resp)
{
	char hn[url->hostNameLen + 1];
	struct hostent *he;
	uint32_t ipAddr;
	int fd, r, chunkedEncoding;
	struct timeval tv;
	struct sockaddr_in sa;
	fd_set rfds, wfds;
	const char *p, *s, *e;
	struct iovec iov;
	const int https = url->port == 443;
	SSL_CTX *sslCtx;
	SSL *ssl;
	enum
	{
		CONNECTING = 0,
		HANDSHAKING_TLS_READ,
		HANDSHAKING_TLS_WRITE,
		READ_RESP,
		SEND_REQ
	} state;

	memcpy((void *)hn, url->hostName, url->hostNameLen);
	hn[url->hostNameLen] = '\0';

	he = gethostbyname(hn);

	if (hn == NULL || he->h_length == 0)
		return -1;

	ipAddr = *(uint32_t *)he->h_addr_list[0]; 	// already in host order


	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return -1;

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)|O_NONBLOCK);

	memset(&sa, 0, sizeof(sa));
	sa.sin_family 		= AF_INET;
	sa.sin_port 		= htons(url->port);
	sa.sin_addr.s_addr 	= (ipAddr);

	if (https)
	{
		sslCtx	= tls_new_sslctx(SSLv23_client_method());
		ssl 	= SSL_new(sslCtx);

		SSL_set_fd(ssl, fd);
		SSL_set_connect_state(ssl);
	}



	if (connect(fd, (struct sockaddr *)&sa, sizeof(sa)) == -1 && errno != EINPROGRESS)
	{
		fprintf(stderr, "failed to connect():%s\n", strerror(errno));
		close(fd);
		return -1;
	}


	FD_ZERO(&rfds);
	FD_ZERO(&wfds);


	// Reuse body
	http_response_ensurecapacity(resp, 8192);
	resp->bodyLen = 0;

	resp->bodyLen += sprintf(resp->body + resp->bodyLen, "%s %.*s HTTP/1.0\r\nConnection: close\r\nHost: %.*s\r\n",
			method, (unsigned)url->pathLen, url->path, (unsigned)url->hostNameLen, url->hostName);

	if (headers)
	{
		http_response_ensurecapacity(resp, string_len(headers));
		memcpy(resp->body + resp->bodyLen, string_data(headers), string_len(headers));
		resp->bodyLen += string_len(headers);
	}


	if (content && string_len(content))
	{
		http_response_ensurecapacity(resp, string_len(content) + 1024);

		resp->bodyLen+=sprintf(resp->body + resp->bodyLen, "Content-Type: application/x-www-form-urlencoded\r\nContent-Length: %u\r\n\r\n", (unsigned)string_len(content));
		memcpy(resp->body + resp->bodyLen, string_data(content), string_len(content));
		resp->bodyLen += string_len(content);
	}
	else
	{
		memcpy(resp->body + resp->bodyLen, "\r\n", 2);
		resp->bodyLen+=2;
	}

	//printf("%.*s\n", (unsigned)resp->bodyLen, resp->body);

	iov.iov_base= resp->body;
	iov.iov_len = resp->bodyLen;

	resp->headersSize = -1;
	resp->contentLen  = -1;


	for (;;)
	{
again:
		tv.tv_sec = 1;
		tv.tv_usec= 0;

		FD_CLR(fd, &wfds);
		FD_SET(fd, &rfds);
		if (iov.iov_len)
			FD_SET(fd, &wfds);

		r = select(fd + 1, &rfds, &wfds, NULL, &tv);

		if (FD_ISSET(fd, &wfds))
		{
			if (state == CONNECTING)
			{
				if (https)
				{
					r = SSL_connect(ssl);
					if (r == -1)
					{
						switch (SSL_get_error(ssl, r))
						{
							case SSL_ERROR_WANT_READ:
								state = HANDSHAKING_TLS_READ;
								break;

							case SSL_ERROR_WANT_WRITE:
								state = HANDSHAKING_TLS_WRITE;
								break;

							default:
								goto onFault;
						}
					}
					else
					{
						state = SEND_REQ;
					}

					goto again;
				}

				state = SEND_REQ;
			}



			if (https)
			{
				if (state == HANDSHAKING_TLS_WRITE)
				{
					if ((r = SSL_connect(ssl)) < 1)
					{
						switch (SSL_get_error(ssl, r))
						{
							case SSL_ERROR_WANT_READ:
								state = HANDSHAKING_TLS_READ;
								break;

							case SSL_ERROR_NONE:
							case SSL_ERROR_WANT_WRITE:
								break;

							default:
								goto onFault;

						}
					}
					else
					{
						state = SEND_REQ;
					}
					goto again;
				}
writeAgain:
				r = SSL_write(ssl, iov.iov_base, iov.iov_len);

				if (r < 0)
				{
					switch (SSL_get_error(ssl, r))
					{
						case SSL_ERROR_NONE:
						case SSL_ERROR_SYSCALL:
							break;
				
						case SSL_ERROR_WANT_WRITE:
						case SSL_ERROR_WANT_READ:
							goto writeAgain;

						default:
							goto onFault;
					}
				}
				else
				{
					iov.iov_len-=r;
					iov.iov_base = ((char *)iov.iov_base) + r;

					if (iov.iov_len == 0)
					{
						// Connected, lets collect the response
						resp->bodyLen = 0;
						state = READ_RESP;
					}
				}

				goto again;
			}



			r = write(fd, iov.iov_base, iov.iov_len);

			if (r == -1)
			{
				if (errno != EINTR && errno != EAGAIN)
					goto onFault;
			}
			else
			{
				iov.iov_len-=r;
				iov.iov_base = ((char *)iov.iov_base) + r;

				if (iov.iov_len == 0)
				{
					// Connected, lets collect the response
					resp->bodyLen = 0;
					state = READ_RESP;
				}
			}
		}
		else if (FD_ISSET(fd, &rfds))
		{
			if (https)
			{
				if (state == HANDSHAKING_TLS_READ)
				{
					if ((r = SSL_connect(ssl)) < 1)
					{
						switch (SSL_get_error(ssl, r))
						{
							case SSL_ERROR_WANT_WRITE:
								state = HANDSHAKING_TLS_WRITE;
								break;

							case SSL_ERROR_WANT_READ:
							case SSL_ERROR_NONE:
								break;

							default:
								break;
						}
					}
					else
					{
						state = SEND_REQ;
					}

					goto again;
				}

				http_response_ensurecapacity(resp, 8192);
readAgain:
				r = SSL_read(ssl, resp->body + resp->bodyLen, 8192);
				if (r < 0)
				{
					switch (SSL_get_error(ssl, r))
					{
						case SSL_ERROR_WANT_READ:
						case SSL_ERROR_WANT_WRITE:
							goto readAgain;

						case SSL_ERROR_NONE:
							errno = EAGAIN;
						
						default:
							errno = EIO;
							break;
					}
				}
				else if (r > 8192)
				{
					// Impossible?
					r 	= -1;
					errno 	= EAGAIN;
				}
			}
			else
			{
				http_response_ensurecapacity(resp, 8192);

				r = read(fd, resp->body + resp->bodyLen, 8192);
			}



			if (r == 0)
			{
				if (resp->headersSize == -1)
					goto onFault;
				else if (chunkedEncoding)
				{
					// Didn't get to read the whole thing
					goto onFault;
				}
				else
				{
					resp->contentLen = resp->bodyLen - resp->headersSize;
					goto onSucc;
				}
			}
			else if (r == -1)
			{
				if (errno != EINTR && errno != EAGAIN)
					goto onFault;
			}
			else
			{
				resp->bodyLen += r;
				resp->body[resp->bodyLen] = '\0';

				if (resp->headersSize == -1)
				{
					int l = 0;

					chunkedEncoding = 0;
					resp->contentLen= -1;
					resp->respCode 	= 0;


					for (p = resp->body;;)
					{
						for (s = p, e = NULL; ;++p)
						{
							if (*p == '\r' && p[1] == '\n')
							{
								e = p;
								p+=2;
								break;
							}
							else if (*p == '\n')
							{
								e = p;
								++p;
								break;
							}
							else if (*p == '\0')
							{
								goto again;
							}
						}

						if (e == s)
						{
							if (l == 0)
							{
								// Expected HTTP/version.revision responseCode responseMsg
								goto onFault;
							}
							else
							{
								resp->headersSize = p - resp->body;
								//printf("Got headers size:%.*s\n", resp->headersSize, resp->body);
								break;
							}

						}
						else if (++l == 1)
						{
							int version, revision;

							if (strncasecmp(s, "HTTP/", 5))
								goto onFault;

							for (version = 0, s+=5; isdigit(*s); ++s)
								version = version * 10 +  (*s - '0');

							if (*s != '.')
								goto onFault;

							for (revision = 0, ++s; isdigit(*s); ++s)
								revision = revision * 10 + (*s - '0');

							if (version > 1 || (version > 0 && revision > 2))
								goto onFault;

							while (isblank(*s))
								++s;

							for (resp->respCode = 0; isdigit(*s); ++s)
								resp->respCode = resp->respCode * 10 + (*s - '0');

						}
						else
						{
							if (!strncasecmp(s, "content-type:", 13))
							{
								for (s+=13; isblank(*s); ++s)
									continue;
							}	
							else if (!strncasecmp(s, "content-len:", 12))
							{
								for (s+=12; isblank(*s); ++s)
									continue;
								for (resp->contentLen = 0; isdigit(*s); ++s)
									resp->contentLen = resp->contentLen * 10 + (*s - '0');

							}
							else if (!strncasecmp(s, "transfer-encoding:", 18))
							{
								for (s+=18; isblank(*s); ++s)
									continue;
								if (!strncasecmp(s, "chunked", 7))
									chunkedEncoding = 1;
							}
						}
					}
				}




				//printf("Reading content %d %u, respcode = %u, bodyLen now = %u\n", (unsigned)resp->contentLen, chunkedEncoding, (unsigned)resp->respCode, (unsigned)resp->bodyLen);

				if (chunkedEncoding)
				{
					const char *p = resp->body + resp->headersSize, *end = resp->body + resp->bodyLen;
					char *out;
					size_t chunkSize, need = 0;

					do
					{
						for (chunkSize = 0;;++p)
						{
							if (*p >= '0' && *p <='9')
								chunkSize = chunkSize * 16 + (*p - '0');
							else if (*p >= 'A' && *p <='F')
								chunkSize = chunkSize * 16 + (*p - 'A' + 10);
							else if (*p >= 'a' && *p <='f')
								chunkSize = chunkSize * 16 + (*p - 'a' + 10);
							else if (*p == ' ' || *p == '\r' || *p == '\n')
							{
								while (*p == ' ')
									++p;

								if (*p == '\r' && p[1] == '\n')
									p+=2;
								else if (*p == '\n')
									++p;
								else
									goto again;
								break;
							}
							else
								goto again;
						}

						if (p + chunkSize >= end)
							goto again;

						for (p+=chunkSize; *p == ' '; ++p)
							continue;

						if (*p == '\r' && p[1] == '\n')
							p+=2;
						else if (*p == '\n')
							++p;
						else
							goto again;
						
						need += chunkSize;
					} while (chunkSize);


					// Could just collected the data as we went along, but its not that expensive to do so here
					// now that we know in advance how much we need to hold

					http_response_ensurecapacity(resp, resp->headersSize + need + 32);

					p = out = resp->body + resp->headersSize;

					do
					{
						for (chunkSize = 0;;++p)
						{
							if (*p >= '0' && *p <='9')
								chunkSize = chunkSize * 16 + (*p - '0');
							else if (*p >= 'A' && *p <='F')
								chunkSize = chunkSize * 16 + (*p - 'A' + 10);
							else if (*p >= 'a' && *p <='f')
								chunkSize = chunkSize * 16 + (*p - 'a' + 10);
							else if (*p == ' ' || *p == '\r' || *p == '\n')
							{
								while (*p == ' ')
									++p;

								if (*p == '\r' && p[1] == '\n')
									p+=2;
								else if (*p == '\n')
									++p;
								else
									goto again;
								
								break;
							}
						}

						memcpy(out, p, chunkSize);
						out+=chunkSize;

						for (p+=chunkSize; *p == ' '; ++p)
							continue;

						if (*p == '\r' && p[1] == '\n')
							p+=2;
						else if (*p == '\n')
							++p;
					} while (chunkSize);

					resp->contentLen = need;
					goto onSucc;
				}

			}
		}
	}


onSucc:
	if (https)
	{
		FreeSSL(ssl);
		tls_destroy_sslctx(sslCtx);
	}
	close(fd);
	return 0;

	
onFault:
	if (https)
	{
		FreeSSL(ssl);
		tls_destroy_sslctx(sslCtx);
	}
	close(fd);
	return -1;
}
