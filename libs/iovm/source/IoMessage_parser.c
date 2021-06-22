/*metadoc Message copyright
        Steve Dekorte; Jonathan Wright, 2002, 2006
*/

/*metadoc Message license
        BSD revised
*/

#include "IoMessage_parser.h"
#include "IoMessage_opShuffle.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoMap.h"
#include "IoNumber.h"
#include "IoState.h"
#include "IoLexer.h"
#include <ctype.h>

#define DATA(self) ((IoMessageData *)IoObject_dataPointer(self))

void IoMessage_parseName(IoMessage *self, IoLexer *lexer);
void IoMessage_parseArgs(IoMessage *self, IoLexer *lexer);
void IoMessage_parseAttached(IoMessage *self, IoLexer *lexer);
void IoMessage_parseNext(IoMessage *self, IoLexer *lexer);
IoMessage *IoMessage_newParseNextMessageChain(void *state, IoLexer *lexer);
void IoMessage_ifPossibleCacheToken_(IoMessage *self, IoToken *p);

void IoMessage_ifPossibleCacheToken_(IoMessage *self, IoToken *p) {
    IoSymbol *method = DATA(self)->name;
    IoObject *r = NULL;

    switch ((int)IoToken_type(p)) {
    case TRIQUOTE_TOKEN:
        r = IoSeq_rawAsUntriquotedSymbol(method);
        break;

    case MONOQUOTE_TOKEN:
        r = IoSeq_rawAsUnescapedSymbol(IoSeq_rawAsUnquotedSymbol(method));
        break;

    case NUMBER_TOKEN:
        r = IONUMBER(IoSeq_asDouble(method));
        break;

    case HEXNUMBER_TOKEN:
        r = IONUMBER(IoSeq_rawAsDoubleFromHex(method));
        break;

    default:
        if (IoSeq_rawEqualsCString_(method, "nil")) {
            r = IONIL(self);
        } else if (IoSeq_rawEqualsCString_(method, "true")) {
            r = IOTRUE(self);
        } else if (IoSeq_rawEqualsCString_(method, "false")) {
            r = IOFALSE(self);
        }
    }

    IoMessage_rawSetCachedResult_(self, r);
}

IoMessage *IoMessage_newFromText_label_(void *state, const char *text,
                                        const char *label) {
    IoSymbol *labelSymbol = IoState_symbolWithCString_((IoState *)state, label);
    return IoMessage_newFromText_labelSymbol_(state, text, labelSymbol);
}

IoMessage *IoMessage_newFromText_labelSymbol_(void *state, const char *text,
                                              IoSymbol *label) {
    IoLexer *lexer;
    IoMessage *msg;

    IoState_pushCollectorPause(state); // needed?

    lexer = IoLexer_new();

    IoLexer_string_(lexer, text);
    IoLexer_lex(lexer);

    msg = IoMessage_newParse(state, lexer);
    IoMessage_opShuffle_(msg);
    IoMessage_label_(msg, label);
    IoLexer_free(lexer);

    IoState_popCollectorPause(state);

    return msg;
}

// -------------------------------

IoMessage *IoMessage_newParse(void *state, IoLexer *lexer) {
    if (IoLexer_errorToken(lexer)) {
        IoMessage *m;
        IoSymbol *errorString;

        // Maybe the nil message could be used here. Or even a NULL.
        IoSymbol *error = IoState_symbolWithCString_(state, "Error");
        m = IoMessage_newWithName_returnsValue_(state, error, error);
        errorString = IoState_symbolWithCString_(
            (IoState *)state, IoLexer_errorDescription(lexer));
        IoLexer_free(
            lexer); // hack for now - the caller should be responsible for this
        IoState_error_(state, m, "compile error: %s", CSTRING(errorString));
    }

    if (IoLexer_topType(lexer) == TERMINATOR_TOKEN) {
        IoLexer_pop(lexer);
    }

    if (IoTokenType_isValidMessageName(IoLexer_topType(lexer))) {
        IoMessage *self = IoMessage_newParseNextMessageChain(state, lexer);

        if (IoLexer_topType(lexer) != NO_TOKEN) {
            // TODO: Exception as the end was expected
            IoState_error_(state, self, "compile error: %s", "unused tokens");
        }

        return self;
    }

    return IoMessage_newWithName_returnsValue_(
        state, IoState_symbolWithCString_((IoState *)state, "nil"),
        ((IoState *)state)->ioNil);
}

/*
typedef struct
{
        void *state;
        IoLexer *lexer;
        Coro *coro;
        Coro *continuation;
        void *result;
} ParseContext;

IoMessage *IoMessage_coroNewParseNextMessageChain(ParseContext *context)
{
        context->result = IoMessage_newParseNextMessageChain(context->state,
context->lexer); Coro_switchTo_(context->coro, context->continuation);
}
*/

IoMessage *IoMessage_newParseNextMessageChain(void *state, IoLexer *lexer) {
    /*
    IoCoroutine *current = IoState_currentCoroutine(state);
    Coro *coro = IoCoroutine_cid(current);
    if (Coro_stackSpaceAlmostGone(coro))
    {
            // need to make Coroutine support a stack of Coros which it frees
    when released
            // return IoCoroutine_internallyChain(current, context,
    IoMessage_...);

            Coro *newCoro = Coro_new();
            ParseContext p = {state, lexer, newCoro, coro, NULL};
            size_t left = Coro_bytesLeftOnStack(coro);
            printf("Warning IoMessage_newParseNextMessageChain doing callc with
    %i bytes left to avoid stack overflow\n", left);

            Coro_startCoro_(coro, newCoro, &p, (CoroStartCallback
    *)IoMessage_coroNewParseNextMessageChain); Coro_free(newCoro); return
    p.result;
    }
    */

    IoMessage *self = IoMessage_new(state);

    if (IoTokenType_isValidMessageName(IoLexer_topType(lexer))) {
        IoMessage_parseName(self, lexer);
    }

    if (IoLexer_topType(lexer) == OPENPAREN_TOKEN) {
        IoMessage_parseArgs(self, lexer);
    }

    if (IoTokenType_isValidMessageName(IoLexer_topType(lexer))) {
        IoMessage_parseNext(self, lexer);
    }

    while (IoLexer_topType(lexer) == TERMINATOR_TOKEN) {
        IoLexer_pop(lexer);

        if (IoTokenType_isValidMessageName(IoLexer_topType(lexer))) {
            IoMessage *eol = IoMessage_newWithName_(
                state, ((IoState *)state)->semicolonSymbol);
            IoMessage_rawSetNext_(self, eol);
            IoMessage_parseNext(eol, lexer);
        }
    }

    return self;
}

void IoMessage_parseName(IoMessage *self, IoLexer *lexer) {
    IoToken *token = IoLexer_pop(lexer);

    DATA(self)->name = IOREF(IOSYMBOL(IoToken_name(token)));

    IoMessage_ifPossibleCacheToken_(self, token);
    IoMessage_rawSetLineNumber_(self, IoToken_lineNumber(token));
    IoMessage_rawSetCharNumber_(self, IoToken_charNumber(token));
}

void IoMessage_parseArgs(IoMessage *self, IoLexer *lexer) {
    IoLexer_pop(lexer);

    if (IoTokenType_isValidMessageName(IoLexer_topType(lexer))) {
        IoMessage *arg = IoMessage_newParseNextMessageChain(IOSTATE, lexer);
        IoMessage_addArg_(self, arg);

        while (IoLexer_topType(lexer) == COMMA_TOKEN) {
            IoLexer_pop(lexer);

            if (IoTokenType_isValidMessageName(IoLexer_topType(lexer))) {
                IoMessage *arg =
                    IoMessage_newParseNextMessageChain(IOSTATE, lexer);
                IoMessage_addArg_(self, arg);
            }
            // Does not actually work because the lexer detects this case and
            // reports the error before we can handle it... else if
            // (IoLexer_topType(lexer) == CLOSEPAREN_TOKEN)
            //{
            //	// Allow the last arg to be empty as in, "foo(a,b,c,)".
            //}
            else {
                // TODO: Exception, missing message
            }
        }
    }

    if (IoLexer_topType(lexer) != CLOSEPAREN_TOKEN) {
        // TODO: Exception, missing close paren
    }
    IoLexer_pop(lexer);
}

void IoMessage_parseNext(IoMessage *self, IoLexer *lexer) {
    IoMessage *next = IoMessage_newParseNextMessageChain(IOSTATE, lexer);
    IoMessage_rawSetNext_(self, next);
}
