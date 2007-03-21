/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOMESSAGE_PARSER_DEFINED
#define IOMESSAGE_PARSER_DEFINED 1

#include "IoMessage.h"
#include "IoLexer.h"

#ifdef __cplusplus
extern "C" {
#endif

IoMessage *IoMessage_newFromText_label_(void *state, const char *text, const char *label);
IoMessage *IoMessage_newParse(void *state, IoLexer *lexer);

#ifdef __cplusplus
}
#endif
#endif
