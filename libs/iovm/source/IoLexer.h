/*   Copyright (c) 2003, Steve Dekorte
docLicense("BSD revised")
 *
 *   The lexer takes source code and produces a token stream. 
 */
 
#ifndef IOLEXER_DEFINED
#define IOLEXER_DEFINED 1

#include "List.h"
#include "Stack.h"
#include "IoToken.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int uchar_t;

typedef struct
{
    char *s;
    char *current;
    List *charLineIndex;
    size_t lineHint;
    size_t maxChar;
    Stack *posStack;
    Stack *tokenStack;
    List *tokenStream;
    size_t resultIndex;
    IoToken *errorToken;
    char *errorDescription;
} IoLexer;

IoLexer *IoLexer_new(void);
void IoLexer_free(IoLexer *self);
void IoLexer_clear(IoLexer *self);

IoToken *IoLexer_errorToken(IoLexer *self);

/* --- lexing --------------- */
void IoLexer_string_(IoLexer *self, const char *string);
int IoLexer_lex(IoLexer *self);
void IoLexer_print(IoLexer *self);
void IoLexer_printTokens(IoLexer *self);
char *IoLexer_errorDescription(IoLexer *self);

/* --- getting results ------ */
IoToken *IoLexer_top(IoLexer *self);
IoTokenType IoLexer_topType(IoLexer *self);
IoToken *IoLexer_pop(IoLexer *self);

/* --- stack management ----- */
char *IoLexer_lastPos(IoLexer *self);
void IoLexer_pushPos(IoLexer *self);
void IoLexer_popPos(IoLexer *self);
void IoLexer_popPosBack(IoLexer *self);

/* --- next/prev character --- */
uchar_t IoLexer_nextChar(IoLexer *self);
uchar_t IoLexer_prevChar(IoLexer *self);

char *IoLexer_current(IoLexer *self);
int IoLexer_onNULL(IoLexer *self);

// grabbing

int IoToken_grabLength(IoLexer *self);
void IoToken_grabTokenType_(IoLexer *self, IoTokenType type);
IoToken *IoLexer_addTokenString_length_type_(IoLexer *self, const char *s1, size_t len, IoTokenType type);

// reading 

int IoLexer_read(IoLexer *self);
void IoLexer_messageChain(IoLexer *self);

// message 

int IoLexer_readMessage(IoLexer *self);
int IoLexer_readPadding(IoLexer *self);
int IoLexer_readOpenParen(IoLexer *self);
int IoLexer_readCloseParen(IoLexer *self);

// symbols 

int IoLexer_readSymbol(IoLexer *self);
int IoLexer_readIdentifier(IoLexer *self);
int IoLexer_readOperator(IoLexer *self);

// comments 

int IoLexer_readComment(IoLexer *self);
int IoLexer_readSlashStarComment(IoLexer *self);
int IoLexer_readSlashSlashComment(IoLexer *self);
int IoLexer_readPoundComment(IoLexer *self);

// quotes 

int IoLexer_readQuote(IoLexer *self);
int IoLexer_readMonoQuote(IoLexer *self);
int IoLexer_readTriQuote(IoLexer *self);

// helpers 

int IoLexer_readTokenChar_type_(IoLexer *self, char c, IoTokenType type);
int IoLexer_readTokenString_(IoLexer *self, const char *s);

int IoLexer_readString_(IoLexer *self, const char *s);
int IoLexer_readChar_(IoLexer *self, char c);
int IoLexer_readCharIn_(IoLexer *self, const char *s);

int IoLexer_readNonReturn(IoLexer *self);
int IoLexer_readNonQuote(IoLexer *self);

// character definitions

int IoLexer_readCharacter(IoLexer *self);

int IoLexer_readOpChar(IoLexer *self);
int IoLexer_readSpecialChar(IoLexer *self);
int IoLexer_readNonColonSpecialChar(IoLexer *self);
int IoLexer_readDigit(IoLexer *self);
int IoLexer_readLetter(IoLexer *self);
int IoLexer_readDigit(IoLexer *self);

int IoLexer_readTerminator(IoLexer *self);
int IoLexer_readTerminatorChar(IoLexer *self);

int IoLexer_readSeparator(IoLexer *self);
int IoLexer_readSeparatorChar(IoLexer *self);

int IoLexer_readWhitespace(IoLexer *self);
int IoLexer_readWhitespaceChar(IoLexer *self);

int IoLexer_readNumber(IoLexer *self);
int IoLexer_readDecimal(IoLexer *self);
int IoLexer_readHexNumber(IoLexer *self);

// parsing 

IoToken *IoLexer_rootMessageToken(IoLexer *self, IoToken **error);

#ifdef __cplusplus
}
#endif
#endif
