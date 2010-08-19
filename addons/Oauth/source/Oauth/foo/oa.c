#include <stdio.h>
#include "http.h"
#include "crypto.h"
#include "aux.h"
#include <time.h>
#include <sys/time.h>
#include "crypto.h"

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
	string_append_urlencoded_rfc3986(&senv->signatureSeed, (uint8_t *)url->path, p - url->path);
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
	struct sha1_context sha1Ctx;
	uint8_t digest[16];

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

	printf("base_string:%.*s\n", (unsigned)string_len(&senv->signatureSeed), string_data(&senv->signatureSeed));

	string_reset(&senv->signatureParamsBuf);
	hmac((uint8_t *)_k, keyLen, (uint8_t *)string_data(&senv->signatureSeed), string_len(&senv->signatureSeed), digest);
	base64_encode(digest, 16, &senv->signatureParamsBuf, 0);

	//printf("base64:[%.*s]\n", (unsigned)string_len(&senv->signatureParamsBuf), string_data(&senv->signatureParamsBuf)); exit(0);
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

l1:
	signctx_dealloc(&senv);
}
	


int main(int argc, char *argv[])
{
	struct url_props url;
	struct http_response resp;
	uint32_t i;


#if 1
	uint8_t md[20];
	char key[] = "Alogaki bleu green hehe oh well what a wonderful world this is";
	char text[] = "Prasino";
	struct sha1_context ctx;

	sha1_init(&ctx);
	sha1_update(&ctx, (uint8_t *)"a", 1);
	sha1_finalize(&ctx, md);

	//hmac((uint8_t *)key, sizeof(key), (uint8_t *)text, sizeof(text), md);

	for (i = 0; i < 20; ++i)
		printf("%u\n", md[i]);

	return 0;

#endif

	tls_init(); // Once is enough

	if (ParseUrl(argv[1], &url))
	{
		fprintf(stderr, "Failed to parse URL\n");
		return 1;
	}	

	printf("Parsed [%.*s][%.*s]\n", (unsigned)url.hostNameLen, url.hostName, (unsigned)url.pathLen, url.path);

	struct string headers;

#if 0
#define CONSUMER_KEY 	"20b8l1JOgtI7jb4pP49sA"
#define CONSUMER_SECRET "w0k414d2SQ8zep12pI6fYEqKBDdaszWDJQfaxLD9QU"
#else
#define CONSUMER_KEY 	"pathfinder.gr"
#define CONSUMER_SECRET "89qWwI7RVVyhx0yGT89zjCpr"
#endif

	string_init(&headers);
	oauth_signandappend_oauth_header("GET", &url, CONSUMER_KEY, CONSUMER_SECRET, NULL, NULL, time(NULL), NULL, 0, NULL, NULL, "gmail", &headers);

	printf("[%.*s]\n", (unsigned)string_len(&headers), string_data(&headers));


	memset(&resp, 0, sizeof(struct http_response));
	if (HttpRequest(&url, "GET", &headers, NULL, &resp))
	{
		fprintf(stderr, "Failed to invoke HTTP request\n");
		return 1;
	}

	printf("Read, content len:%d\n", resp.contentLen);
	printf("[%.*s]\n", (unsigned)resp.bodyLen, resp.body);

	return 0;
}

