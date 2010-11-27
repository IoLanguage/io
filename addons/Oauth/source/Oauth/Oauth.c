/*
 *	Ref: http://code.google.com/apis/accounts/docs/OAuth.html
 *
 *	1. Request an authorized token from service
 *	2. Servie asks the user to grand access to the required data
 *	3. Application gets an authorized request token from the authorization service
 *	4. Application exchanges authorization request token for an access token
 *	5. Access token is used to request data from service
 */
#include <stdio.h>
#include "http.h"
#include "crypto.h"

/*
	"aux" is a reserved device name on Windows and causes both git-bash and CMake to error out when accessing the file name.
	I've changed the file names to _aux.h and _aux.c to fix it. My apologies to the original author of OAuth.  --Dennis Ferron
*/
#include "_aux.h"

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include "crypto.h"
#include "tls.h"
#include "Oauth.h"

struct signature_param
{
	char *name;
	const char *value;
	size_t len;
};

struct signctx
{
	struct allocator mAllocator;
	struct vector signatureParams;
	struct string signatureParamsBuf, signatureSeed;
};


static void signctx_init(struct signctx *self)
{
	allocator_init(&self->mAllocator, 256);
	vector_init(&self->signatureParams, sizeof(struct signature_param));
	string_init(&self->signatureParamsBuf);
	string_init(&self->signatureSeed);
}

static void signctx_dealloc(struct signctx *self)
{
	allocator_dealloc(&self->mAllocator);
	vector_dealloc(&self->signatureParams);
	string_dealloc(&self->signatureParamsBuf);
	string_dealloc(&self->signatureSeed);
}

static void append_signature_param(struct signctx *senv, char *name, const uint8_t nameLen, const char *v, const size_t vl)
{
	struct signature_param *sp = vector_pushnew(&senv->signatureParams);

	sp->name 	= allocator_alloc(&senv->mAllocator, nameLen + 1);
	memcpy(sp->name, name, nameLen);
	sp->value  	= v;
	sp->len 	= vl;
}

static void append_signature_params(struct signctx *senv, const char *p, const char *e)
{
	struct signature_param *sp;
	const char *n, *v;

	for (;;)
	{
		for (n = p; p != e && *p != '='; ++p)
			continue;

		if (p != e)
		{
			if (*p != '=')
				break;

			sp = vector_pushnew(&senv->signatureParams);
			sp->name = allocator_alloc(&senv->mAllocator, (p - n) + 1);
			memcpy(sp->name, n, p - n);
			sp->name[p - n] = '\0';

			for (++p, v = p; p != e && *p != '&'; ++p)
				continue;
			
			sp->value 	= (char *)v;
			sp->len 	= p-v;

			if (p == e)
				break;

			++p;
		}
		else
		{
			if (p != n)
			{
				sp = vector_pushnew(&senv->signatureParams);
				sp->name 	= allocator_alloc(&senv->mAllocator, (p - n) + 1);
				memcpy(sp->name, n, p - n);
				sp->name[p - n] = '\0';
				sp->value 	= NULL;
				sp->len 	= 0;
			}

			break;
		}
	}
}


static void init_signature_seed(struct signctx *senv, const char *reqMethod, const struct url_props *url)
{
	const char *e = url->path + url->pathLen, *p = url->path;

	string_reset(&senv->signatureParamsBuf);
	vector_reset(&senv->signatureParams);

	while (p != e && *p != '?')
		++p;
	
	if (p != e)
		append_signature_params(senv, p + 1, e);

	string_reset(&senv->signatureSeed);
	string_appendfmt(&senv->signatureSeed, "%s&%s%%3A%%2F%%2F%.*s", reqMethod, url->port == 443 ? "https": "http", url->hostNameLen, url->hostName);
	string_append_urlencoded_rfc3986(&senv->signatureSeed, url->path, p - url->path);
}

static inline int bySignatureParamName(const void *a, const void *b)
{
	const struct signature_param *p1 = (struct signature_param *)a, *p2 = (struct signature_param *)b;

	return strcmp(p1->name, p2->name);
}

static void build_signature(struct signctx *senv, const char *method, const char *consumerSecret, const char *tokenSecret)
{
	struct signature_param *sp;
	const size_t nParams = vector_size(&senv->signatureParams);
	uint32_t i;
	uint8_t digest[20];

	qsort(vector_values(&senv->signatureParams), vector_size(&senv->signatureParams), senv->signatureParams.item_size, bySignatureParamName);
	string_append(&senv->signatureSeed, "&", 1);

	if (nParams)
	{
		i = 0;
		do
		{
			sp = (struct signature_param *)vector_value(&senv->signatureParams, i);
			string_appendfmt(&senv->signatureSeed, "%s%%3D", sp->name);
			string_append_urlencoded_rfc3986(&senv->signatureSeed, sp->value, sp->len);
			string_append(&senv->signatureSeed, "%26", 3);

		} while (++i != nParams);
		string_adjustlen(&senv->signatureSeed, 3);
	}


	char _k[256];
	const size_t keyLen = sprintf(_k, "%s&%s", consumerSecret, tokenSecret);

	string_reset(&senv->signatureParamsBuf);
	hmac((uint8_t *)_k, keyLen, (uint8_t *)string_data(&senv->signatureSeed), string_len(&senv->signatureSeed), digest);
	base64_encode(digest, 20, &senv->signatureParamsBuf, 0);
}



void oauth_signandappend_oauth_header(const char *reqMethod, struct url_props *url, const char *consumerKey, const char *consumerSecret, const char *authToken, const char *authTokenSecret, const time_t now, const char *postContent, const size_t postContentLen, const char *oauthCallback, const char *oauthVerifier, const char *oauthScope, struct string *out)
{
	struct signctx senv;
	char _tb[256];
	const size_t _tbLen = sprintf(_tb, "%u", (unsigned)now);
	struct timeval tv;
	struct md5_context md5Ctx;

	signctx_init(&senv);
	init_signature_seed(&senv, reqMethod, url);

	if (postContentLen)
		append_signature_params(&senv, postContent, postContent + postContentLen);


	string_append(out, "Authorization: OAuth ", 21);

	append_signature_param(&senv, "oauth_consumer_key", 18, consumerKey, strlen(consumerKey));
	string_appendfmt(out, "oauth_consumer_key=\"%s\"", consumerKey);

	append_signature_param(&senv, "oauth_signature_method", 22, "HMAC-SHA1", 9);
	string_append(out, ",oauth_signature_method=\"HMAC-SHA1\"", 35);


        string_appendfmt(out,",oauth_timestamp=\"%u\"", (uint32_t)now);
        append_signature_param(&senv, "oauth_timestamp", 15, _tb, _tbLen);
                
        gettimeofday(&tv, NULL);
                        
        uint8_t digest[16];
        char digestAlpha[33];

	md5_init(&md5Ctx);
        md5_update(&md5Ctx, (uint8_t *)&tv.tv_sec, sizeof(tv.tv_sec));
        md5_update(&md5Ctx, (uint8_t *)&tv.tv_usec, sizeof(tv.tv_usec));
        md5_update(&md5Ctx, (uint8_t *)consumerKey, strlen(consumerKey));
        md5_update(&md5Ctx, (uint8_t *)"io.language", 11);
	md5_finalize(&md5Ctx, digest);
                
        md5_string((char *)digest, digestAlpha);
        const size_t digestAlphaLen = strlen(digestAlpha);      // 32
                
	string_appendfmt(out, ",oauth_nonce=\"%.*s\"",  digestAlphaLen, digestAlpha);
        append_signature_param(&senv, "oauth_nonce", 11, digestAlpha, digestAlphaLen);

        string_appendfmt(out, ",oauth_version=\"1.0\"", 20);
        append_signature_param(&senv, "oauth_version", 13, "1.0", 3);
                
        if (authToken && *authToken != '\0')
        {
                string_appendfmt(out, ",oauth_token=\"%s\"", authToken);
                append_signature_param(&senv, "oauth_token", 11, authToken, strlen(authToken));
        }       

        if (oauthCallback && *oauthCallback != '\0')
        {
                string_appendfmt(out, ",oauth_callback=\"%s\"", oauthCallback);
                append_signature_param(&senv, "oauth_callback", 14, oauthCallback, strlen(oauthCallback));
        }
                
        if (oauthVerifier && *oauthVerifier != '\0')
        {
                string_appendfmt(out, ",oauth_verifier=\"%s\"", oauthVerifier);
                append_signature_param(&senv, "oauth_verifier", 14, oauthVerifier, strlen(oauthVerifier));
        }

        if (oauthScope && *oauthScope != '\0')
        {
                string_appendfmt(out, ",scope=\"%s\"", oauthScope);
                append_signature_param(&senv, "scope", 5, oauthScope, strlen(oauthScope));
        }
	
        build_signature(&senv, "HMAC-SHA1", consumerSecret, authTokenSecret ? authTokenSecret : "");

        string_append(out, ",oauth_signature=\"", 18);
	string_append_urlencoded_rfc3986(out, string_data(&senv.signatureParamsBuf), string_len(&senv.signatureParamsBuf));
	string_append(out, "\"\r\n", 3);

	signctx_dealloc(&senv);
}
	












/*
 *	consumerKey and consumerSecret are provided by the service when the application is registered
 *	The user should authorize the token by visiting e.g https://twitter.com/oauth/authorize?oauth_token=authToken
 *	Once that is done, the user is redirected to the callback url:
 *	http://domain/path?oauth_token=0Aa5BuS45OUkWyKyc0Y12JfbWB5g9vYgcJLq2Zvsbo
 *	(oauth_token argument is provided by service)
 *
 *	We now have to exchange it for an ccess token
 */
int oauth_get_request_token(const char *fullUrl, 
	const char *consumerKey, 
	const char *consumerSecret, 
	struct string *oauthToken, 
	struct string *oauthTokenSecret)
{
	struct url_props url;
	struct string headers;
	struct http_response resp;
	const char *p, *e;

	if (parse_url(fullUrl, &url))
		return -1;

	string_reset(oauthToken);
	string_reset(oauthTokenSecret);

	string_init(&headers);
	oauth_signandappend_oauth_header("GET", &url, consumerKey, consumerSecret, NULL, NULL, time(NULL), NULL, 0, NULL, NULL, NULL, &headers);

	http_response_init(&resp);
	
	printf("url = 'https://%s'\n", url.hostName);
	printf("headers = '%s'\n", headers.data);
	
	if (http_request(&url, "GET", &headers, NULL, &resp))
	{
		printf("request failure [%s]", resp.body);
		goto onFailure;
	}
	p = resp.body + resp.headersSize;
	e = p + resp.contentLen;

	printf("1 Parsing [%.*s]\n", e - p, p);

	while (p != e)
	{
		const char *name = p;

		while (p != e)
		{
			if (*p == '=')
				break;
			++p;
		}


		if (p != e)
		{
			const size_t nameLen 	= p - name;
			const char *value 	= ++p;
			size_t valueLen;

			printf("2 [%.*s]\n", (signed)nameLen, name);

			for (; p != e && *p != '&'; ++p)
				continue;

			valueLen = p - value;

			if (nameLen == 11 && memcmp(name, "oauth_token", 11) == 0)
				string_append(oauthToken, value, valueLen);
			else if (nameLen == 18 && memcmp(name, "oauth_token_secret", 18) == 0)
				string_append(oauthTokenSecret, value, valueLen);

			printf("value =[%.*s]\n", (signed)valueLen, value);

			if (p != e)
				++p;
			else
				break;
		}
		else
			break;
	}

	if (string_len(oauthToken) && string_len(oauthTokenSecret))
	{
		string_dealloc(&headers);
		http_response_dealloc(&resp);
		return 0;
	}

onFailure:
	string_dealloc(&headers);
	http_response_dealloc(&resp);
	return -1;
}

/*
 *	Once the user has authorized the request key, we can exchange it for an access key
 *	This function implements the logic required to do so.
 *	Once you get back the new/exchanged accessToken and accessSecret you can use it for issuing requests
 *	to the sevice from now on
 */ 
int oauth_exchange_reqtoken(const char *fullUrl, const char *consumerKey, const char *consumerSecret, const char *oauthToken, const char *oauthTokenSecret, struct string *accessToken, struct string *accessSeret)
{
	struct url_props url;
	struct string headers;
	struct http_response resp;
	const char *p, *e;

	if (parse_url(fullUrl, &url))
		return -1;

	string_reset(accessToken);
	string_reset(accessSeret);

	string_init(&headers);
	oauth_signandappend_oauth_header("GET", &url, consumerKey, consumerSecret, oauthToken, oauthTokenSecret, time(NULL), NULL, 0, NULL, NULL, NULL, &headers);

	http_response_init(&resp);
	if (http_request(&url, "GET", &headers, NULL, &resp))
		goto onFailure;

	p = resp.body + resp.headersSize;
	e = p + resp.contentLen;

	printf("3 Parsing [%.*s]\n", e - p, p);

	while (p != e)
	{
		const char *name = p;

		while (p != e)
		{
			if (*p == '=')
				break;
			++p;
		}


		if (p != e)
		{
			const size_t nameLen 	= p - name;
			const char *value 	= ++p;
			size_t valueLen;

			printf("[%.*s]\n", (signed)nameLen, name);

			for (; p != e && *p != '&'; ++p)
				continue;

			valueLen = p - value;

			if (nameLen == 11 && memcmp(name, "oauth_token", 11) == 0)
				string_append(accessToken, value, valueLen);
			else if (nameLen == 18 && memcmp(name, "oauth_token_secret", 18) == 0)
				string_append(accessSeret, value, valueLen);

			printf("value =[%.*s]\n", (signed)valueLen, value);

			if (p != e)
				++p;
			else
				break;
		}
		else
			break;
	}

	if (string_len(accessToken) && string_len(accessSeret))
	{
		string_dealloc(&headers);
		http_response_dealloc(&resp);
		return 0;
	}

onFailure:
	string_dealloc(&headers);
	http_response_dealloc(&resp);
	return -1;

}


/*
 *	The actual HTTP request. Make sure you http_response_dealloc(resp) on return
 *	Make sure you have initialized it before providing it to this function
 */
int oauth_http_request(const char *reqMethod, 
	const char *fullUrl, 
	const char *content, 
	const size_t contentLen, 
	const char *consumerKey, 
	const char *consumerSecret, 
	const char *accessToken, 
	const char *accessSecret, 
	struct http_response *resp)
{
	struct url_props url;
	struct string headers;

	if (parse_url(fullUrl, &url))
		return -1;

	string_init(&headers);
	oauth_signandappend_oauth_header(reqMethod, &url, consumerKey, consumerSecret, accessToken, accessSecret, time(NULL), content, contentLen, NULL, NULL, NULL, &headers);

	http_response_init(resp);
	if (http_request(&url, "GET", &headers, NULL, resp))
		goto onFailure;


	string_dealloc(&headers);
	return 0;

onFailure:
	string_dealloc(&headers);
	return -1;
}


// ----------------------------

struct string *string_new(void)
{
	struct string *self = calloc(1, sizeof(struct string));
	string_init(self);
	string_append(self, "\0", 1);
	self->len = 0;
	return self;
}

void string_set(struct string *self, char *s)
{
	string_append(self, s, strlen(s));
}

// ----------------------------

static int global_ttlInitialized = 0;

Oauth *Oauth_new(void)
{
	if (!global_ttlInitialized) 
	{
		global_ttlInitialized = 1;
		tls_init();
		printf("warning: Oauth_new() tls_init()\n");
	}
	
	Oauth *self = calloc(1, sizeof(Oauth));
	
	self->consumer_key = string_new();
	self->consumer_secret = string_new();
	
	self->oauth_token = string_new();
	self->oauth_secret = string_new();

	self->access_key = string_new();
	self->access_secret = string_new();
	
	self->responseData = string_new();

	return self;
}

void Oauth_free(Oauth* self)
{
	string_dealloc(self->consumer_key);
	string_dealloc(self->consumer_secret);
	
	string_dealloc(self->oauth_token);
	string_dealloc(self->oauth_secret);
	
	string_dealloc(self->access_key);
	string_dealloc(self->access_secret);
	
	string_dealloc(self->responseData);
	free(self);
}

void Oauth_setConsumerKey_(Oauth* self, char *k)	{ string_set(self->consumer_key, k); }
struct string *Oauth_consumerKey(Oauth* self)		{ return self->consumer_key; }

void Oauth_setConsumerSecret_(Oauth* self, char *s) { string_set(self->consumer_secret, s); }
struct string *Oauth_consumerSecret(Oauth* self)    { return self->consumer_secret; }


void Oauth_setOauthToken_(Oauth* self, char *s)		{ string_set(self->oauth_token, s); }
struct string *Oauth_oauthToken(Oauth* self)		{ return self->oauth_token; }

void Oauth_setOauthSecret_(Oauth* self, char *s)	{ string_set(self->oauth_secret, s); }
struct string *Oauth_oauthSecret(Oauth* self)		{ return self->oauth_secret; }


void Oauth_setAccessKey_(Oauth* self, char *s)		{ string_set(self->access_key, s); }
struct string *Oauth_accessKey(Oauth* self)			{ return self->access_key; }

void Oauth_setAccessSecret_(Oauth* self, char *s)	{ string_set(self->access_secret, s); }
struct string *Oauth_accessSecret(Oauth* self)		{ return self->access_secret; }


void Oauth_getOauthTokenAndSecretFromUrl_(Oauth *self, char *url)
{
	// https://api.twitter.com/oauth/request_token
	printf("Oauth_getOauthTokenAndSecretFromUrl_ \n");
	printf("	consumer_key    = '%s' \n", string_data(self->consumer_key));
	printf("	consumer_secret = '%s' \n", string_data(self->consumer_secret));
	oauth_get_request_token(url, 
		string_data(self->consumer_key), 
		string_data(self->consumer_secret), 
		self->oauth_token, 
		self->oauth_secret);
}

void Oauth_getAccessKeyAndSecretFromUrl_(Oauth *self, char *url)
{
	// https://www.google.com/accounts/OAuthAuthorizeToken?oauth_token=OBTAINED_REQUEST_TOKEN
	oauth_exchange_reqtoken(url, 
		string_data(self->consumer_key), 
		string_data(self->consumer_secret), 
		string_data(self->oauth_token), 
		string_data(self->oauth_secret), 
		self->access_key, 
		self->access_secret);
}

void Oauth_requestUrl_(Oauth *self, char *url, char *postContent, int contentLength)
{
	// http://api.twitter.com/1/statuses/user_timeline.json
	char *method = postContent == NULL ? "GET" : "POST";
	struct http_response resp;
	http_response_init(&resp);	
	oauth_http_request(method, 
		url, 
		postContent, contentLength, 
		string_data(self->consumer_key), 
		string_data(self->consumer_secret), 
		string_data(self->access_key), 
		string_data(self->access_secret), 
		&resp);
	// printf("got response code %u, content: [%.*s]\n", (unsigned)resp.respCode, (unsigned)resp.contentLen, resp.body + resp.headersSize);
	self->responseData->len = 0;
	string_append(self->responseData, resp.body, resp.headersSize + resp.contentLen);
	http_response_dealloc(&resp);
}

struct string *Oauth_responseData(Oauth *self)
{
	return self->responseData;
}

/*

int main(int argc, char *argv[])
{
	struct http_response resp;
	struct string token, secret;


	tls_init(); 	// This should be invoked once 
	// Eg. via a static global
	// static int __dummy = tls_init();


	string_init(&token);
	string_init(&secret);
	http_response_init(&resp);

	// Obtain a request token. Consumer key and consumer secret are provided by the service/e.g twitter
	// https://api.twitter.com/oauth/request_token
	//oauth_get_request_token(argv[1], CONSUMER_KEY, CONSUMER_SECRET, &token, &secret);


	// Once it is obtained, go to to  https://www.google.com/accounts/OAuthAuthorizeToken?oauth_token=OBTAINED_REQUEST_TOKEN
	// and it will be confirmed.
	// Then, the following function will exhcange the request token/secret with access/token secret
	//oauth_exchange_reqtoken(argv[1], CONSUMER_KEY, CONSUMER_SECRET, OAUTH_TOKEN, OAUTH_TOKENSECRET, &token, &secret);


	// Now that we have the access token/secret pair, we can invoke any HTTP request.
	// http://api.twitter.com/1/statuses/user_timeline.json
	// http_response_init(&resp);
	//oauth_http_request("GET", argv[1], NULL, 0, CONSUMER_KEY, CONSUMER_SECRET, accessKey, accessSecret &resp);
	// printf("got response code %u, content: [%.*s]\n", (unsigned)resp.respCode, (unsigned)resp.contentLen, resp.body + resp.headersSize);
	// http_response_release(&resp);

	return 0;
}
*/
