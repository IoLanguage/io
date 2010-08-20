#ifndef IO_OAUTH_H
#define IO_OAUTH_H
#include "aux.h"
#include "http.h"

typedef struct
{
	struct string *consumer_key;
	struct string *consumer_secret;
	
	struct string *oauth_token;
	struct string *oauth_secret;
	
	struct string *access_key;
	struct string *access_secret;

	struct string *responseData;
} Oauth;

Oauth* Oauth_new(void);
void Oauth_free(Oauth* self);


void Oauth_setConsumerKey_(Oauth* self, char *k);
struct string *Oauth_consumerKey(Oauth* self);

void Oauth_setConsumerSecret_(Oauth* self, char *s);
struct string *Oauth_consumerSecret(Oauth* self);


void Oauth_setOauthToken_(Oauth* self, char *s);
struct string *Oauth_oauthToken(Oauth* self);

void Oauth_setOauthSecret_(Oauth* self, char *s);
struct string *Oauth_oauthSecret(Oauth* self);


void Oauth_setAccessKey_(Oauth* self, char *s);
struct string *Oauth_accessKey(Oauth* self);

void Oauth_setAccessSecret_(Oauth* self, char *s);
struct string *Oauth_accessSecret(Oauth* self);


void Oauth_getOauthTokenAndSecretFromUrl_(Oauth *self, char *url);
void Oauth_getAccessKeyAndSecretFromUrl_(Oauth *self, char *url);

void Oauth_requestUrl_(Oauth *self, char *url, char *postContent, int contentLength);
struct string *Oauth_responseData(Oauth *self);

/*
int oauth_get_request_token(const char *fullUrl, const char *consumerKey, const char *consumerSecret, struct string *oauthToken, struct string *oauthTokenSecret);
int oauth_exchange_reqtoken(const char *fullUrl, const char *consumerKey, const char *consumerSecret, const char *oauthToken, const char *oauthTokenSecret, struct string *accessToken, struct string *accessSeret);
int oauth_http_request(const char *reqMethod, const char *fullUrl, const char *content, const size_t contentLen, const char *consumerKey, const char *consumerSecret, const char *accessToken, const char *accessSecret, struct http_response *resp);
*/

#endif
