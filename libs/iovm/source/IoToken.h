/*   Copyright (c) 2003, Steve Dekorte
docLicense("BSD revised")
 */
 
#ifndef IOTOKEN_DEFINED
#define IOTOKEN_DEFINED 1

#include "List.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{ 
    NO_TOKEN, 

    OPENPAREN_TOKEN, 
    COMMA_TOKEN, 
    CLOSEPAREN_TOKEN, 

    MONOQUOTE_TOKEN, 
    TRIQUOTE_TOKEN, 

    IDENTIFIER_TOKEN, 
    TERMINATOR_TOKEN, 

    COMMENT_TOKEN,
    NUMBER_TOKEN, 
    HEXNUMBER_TOKEN
} IoTokenType;
 
typedef struct IoToken IoToken;

struct IoToken
{
    char *name;
    size_t length;
    IoTokenType type;
    int charNumber;
    int lineNumber;
    IoToken *nextToken;
    char *error;
};

IoToken *IoToken_new(void);
void IoToken_free(IoToken *self);

const char *IoToken_typeName(IoToken *self);

void IoToken_name_length_(IoToken *self, const char *name, size_t len);
void IoToken_name_(IoToken *self, const char *name);
char *IoToken_name(IoToken *self);
#define IOTOKEN_NAME(self)   (self->name ? self->name : (char *)"")

void IoToken_error_(IoToken *self, const char *name);
char *IoToken_error(IoToken *self);

int IoToken_nameIs_(IoToken *self, const char *name);
IoTokenType IoToken_type(IoToken *self);

int IoToken_lineNumber(IoToken *self);
int IoToken_charNumber(IoToken *self);

void IoToken_quoteName_(IoToken *self, const char *name);
void IoToken_type_(IoToken *self, IoTokenType type);
void IoToken_nextToken_(IoToken *self, IoToken *nextToken);
void IoToken_print(IoToken *self);
void IoToken_printSelf(IoToken *self);

int IoTokenType_isValidMessageName(IoTokenType self);

#ifdef __cplusplus
}
#endif
#endif
