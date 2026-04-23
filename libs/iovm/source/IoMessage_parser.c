/*metadoc Message copyright
        Steve Dekorte; Jonathan Wright, 2002, 2006
*/

/*metadoc Message license
        BSD revised
*/

/*cmetadoc Message description
Recursive-descent parser that turns an IoLexer token stream into an
IoMessage tree. Each message is { name, args, next } — parseName
consumes one valid-message-name token, parseArgs recursively parses
comma-separated argument chains inside matched parens, parseNext
attaches the tail of the current chain, and semicolon-terminated
expressions become an explicit ";" message linked via rawSetNext.
Number, quote, nil/true/false tokens are cached into the message's
cachedResult via IoMessage_ifPossibleCacheToken_ so the evaluator
skips re-parsing them at runtime. Once the tree is built it is handed
to IoMessage_opShuffle_ for operator precedence rewriting before
being handed to the evaluator.
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

/*cdoc Message IoMessage_ifPossibleCacheToken_(self, p)
Converts a literal token into its runtime Io value and stores it as the
message's cachedResult so evaluation can return the value without a
re-parse. Handles the four literal token types plus the three reserved
words nil/true/false that are identifiers at the lexer level. The
cachedResult short-circuits the evaluator's performFunc lookup in
IoMessage_locals_performOn_.
*/
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

/*cdoc Message IoMessage_newFromText_label_(state, text, label)
Convenience wrapper that interns `label` as an IoSymbol and delegates
to IoMessage_newFromText_labelSymbol_. This is the usual entry point
for C code that has a bare const char * filename to attach to a
message tree (e.g. IoState_doCString_).
*/
IoMessage *IoMessage_newFromText_label_(void *state, const char *text,
                                        const char *label) {
    IoSymbol *labelSymbol = IoState_symbolWithCString_((IoState *)state, label);
    return IoMessage_newFromText_labelSymbol_(state, text, labelSymbol);
}

/*cdoc Message IoMessage_newFromText_labelSymbol_(state, text, label)
Drives the full compile pipeline: lex -> parse -> opShuffle -> label.
The collector is paused over the whole run so intermediate IoMessages
aren't collected while the tree is being built. The lexer is freed
before return; the caller receives a ready-to-evaluate message root.
*/
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

/*cdoc Message IoMessage_newParse(state, lexer)
Top-level parse entry point. Reports a compile error via IoState_error_
if the lexer flagged one, consumes a leading TERMINATOR, then recurses
into IoMessage_newParseNextMessageChain for the actual chain. Returns
a synthesized nil message for an empty input, and a compile error if
tokens remain after the chain (dangling punctuation).
*/
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

/*cdoc Message IoMessage_newParseNextMessageChain(state, lexer)
Parses one message chain: optional name, optional parenthesised args,
optional attached next message, and any number of semicolon-separated
continuations (each stitched together with an explicit ";" message
that uses state->semicolonSymbol). This mirrors the grammar that
IoMessage_opShuffle later rewrites for operator precedence.
*/
IoMessage *IoMessage_newParseNextMessageChain(void *state, IoLexer *lexer) {
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

/*cdoc Message IoMessage_parseName(self, lexer)
Pops the current token and installs it as the message's name. Runs
IoMessage_ifPossibleCacheToken_ so literals pick up their cached value,
and records lineNumber/charNumber for diagnostics and stack traces.
*/
void IoMessage_parseName(IoMessage *self, IoLexer *lexer) {
    IoToken *token = IoLexer_pop(lexer);

    DATA(self)->name = IOREF(IOSYMBOL(IoToken_name(token)));

    IoMessage_ifPossibleCacheToken_(self, token);
    IoMessage_rawSetLineNumber_(self, IoToken_lineNumber(token));
    IoMessage_rawSetCharNumber_(self, IoToken_charNumber(token));
}

/*cdoc Message IoMessage_parseArgs(self, lexer)
Consumes the OPENPAREN and all comma-separated argument chains,
appending each parsed sub-chain to self's args list. Each arg is
itself a full message chain, recursively parsed by
IoMessage_newParseNextMessageChain. Closes on CLOSEPAREN; missing
close paren and missing-argument cases would be flagged by the lexer
earlier via IoLexer_readMessage_error.
*/
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

/*cdoc Message IoMessage_parseNext(self, lexer)
Parses the next message in the chain (after the current name/args)
and links it via IoMessage_rawSetNext_. This is what turns
`a b c` into three linked messages rather than three separate
top-level expressions.
*/
void IoMessage_parseNext(IoMessage *self, IoLexer *lexer) {
    IoMessage *next = IoMessage_newParseNextMessageChain(IOSTATE, lexer);
    IoMessage_rawSetNext_(self, next);
}
