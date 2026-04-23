
// metadoc Token copyright Steve Dekorte 2002
// metadoc Token license BSD revised

/*cmetadoc Token description
Plain-C token node produced by IoLexer as it scans source text. A token
carries its spelling (name/length), an IoTokenType classification, a
lineNumber/charNumber for diagnostics, an optional error string, and a
forward-link `nextToken` that the lexer uses to splice tokens into a
singly-linked chain parallel to the per-frame List tokenStream. Tokens
are owned by the lexer and freed via IoToken_free; they are not IoObjects
and are not GC-managed — the parser in IoMessage_parser.c consumes them
one at a time and translates each into an IoMessage.
*/

#include "IoToken.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*cdoc Token IoToken_new(void)
Allocates a zeroed IoToken with name unset and charNumber sentinel -1.
Callers populate fields via IoToken_name_length_ / IoToken_type_ as the
lexer grabs a matched span.
*/
IoToken *IoToken_new(void) {
    IoToken *self = (IoToken *)io_calloc(1, sizeof(IoToken));
    self->name = NULL;
    self->charNumber = -1;

    return self;
}

/*cdoc Token IoToken_free(self)
Releases the name and error buffers (both io_malloced) and the token
itself. Does not recurse into nextToken — the lexer's tokenStream List
iterates and frees each element explicitly via IoLexer_clear.
*/
void IoToken_free(IoToken *self) {
    if (self->name)
        io_free(self->name);
    if (self->error)
        io_free(self->error);
    io_free(self);
}

/*cdoc Token IoToken_typeName(self)
Maps an IoTokenType enum to a human-readable name for debugging and for
the Io-visible Compiler tokensForString result. Kept in sync with the
IoTokenType definition in IoToken.h.
*/
const char *IoToken_typeName(IoToken *self) {
    switch (self->type) {
    case NO_TOKEN:
        return "NoToken";
    case OPENPAREN_TOKEN:
        return "OpenParen";
    case COMMA_TOKEN:
        return "Comma";
    case CLOSEPAREN_TOKEN:
        return "CloseParen";
    case MONOQUOTE_TOKEN:
        return "MonoQuote";
    case TRIQUOTE_TOKEN:
        return "TriQuote";
    case IDENTIFIER_TOKEN:
        return "Identifier";
    case TERMINATOR_TOKEN:
        return "Terminator";
    case COMMENT_TOKEN:
        return "Comment";
    case NUMBER_TOKEN:
        return "Number";
    case HEXNUMBER_TOKEN:
        return "HexNumber";
    }
    return "UNKNOWN_TOKEN";
}

/*cdoc Token IoToken_name_length_(self, name, len)
Copies a non-NUL-terminated span of len bytes into the token's owned
name buffer, reallocating as needed and NUL-terminating. This is the
hot path called by IoLexer_grabTokenType_ once a span has been matched.
*/
void IoToken_name_length_(IoToken *self, const char *name, size_t len) {
    self->name = strncpy(io_realloc(self->name, len + 1), name, len);
    self->name[len] = (char)0;
    self->length = len;
}

/*cdoc Token IoToken_name_(self, name)
Convenience form that copies a NUL-terminated C string. Used for
synthesized tokens (e.g. the ";" terminator, the "squareBrackets" /
"curlyBrackets" group-name tokens) rather than raw source spans.
*/
void IoToken_name_(IoToken *self, const char *name) {
    self->name = strcpy((char *)io_realloc(self->name, strlen(name) + 1), name);
    self->length = strlen(name);
}

char *IoToken_name(IoToken *self) {
    return self->name ? self->name : (char *)"";
}

/*cdoc Token IoToken_error_(self, error)
Attaches a diagnostic message to the token. The lexer sets this when
it detects an unterminated quote, an unmatched bracket, or a stray
character — the parser later reads errorToken back via the lexer to
build the "compile error" Exception.
*/
void IoToken_error_(IoToken *self, const char *error) {
    self->error =
        strcpy((char *)io_realloc(self->error, strlen(error) + 1), error);
}

char *IoToken_error(IoToken *self) {
    return self->error ? self->error : (char *)"";
}

/*cdoc Token IoToken_nameIs_(self, name)
Strict string comparison of the token's spelling against the given C
literal. The empty-vs-nonempty guard avoids a strcmp on a synthesized
empty-name token produced for unnamed groups like `[...]`.
*/
int IoToken_nameIs_(IoToken *self, const char *name) {
    if (strlen(self->name) == 0 && strlen(name) != 0) {
        return 0;
    }
    // return !strncmp(self->name, name, self->length);
    return !strcmp(self->name, name);
}

IoTokenType IoToken_type(IoToken *self) { return self->type; }

int IoToken_lineNumber(IoToken *self) { return self->lineNumber; }

int IoToken_charNumber(IoToken *self) { return self->charNumber; }

/*cdoc Token IoToken_quoteName_(self, name)
Rewrites the token's spelling by wrapping it in double quotes. Used
when coercing an identifier into a string literal for generated assign
messages (see IoMessage_opShuffle's setSlot rewriting).
*/
void IoToken_quoteName_(IoToken *self, const char *name) {
    char *old = self->name;
    size_t length = strlen(name) + 3;
    self->name = io_calloc(1, length);
    snprintf(self->name, length, "\"%s\"", name);

    if (old) {
        io_free(old);
    }
}

void IoToken_type_(IoToken *self, IoTokenType type) { self->type = type; }

/*cdoc Token IoToken_nextToken_(self, nextToken)
Wires `self` to `nextToken` in the lexer's forward chain, freeing any
previously attached successor. The self-identity guard fires on a lexer
bug and aborts the process rather than silently looping.
*/
void IoToken_nextToken_(IoToken *self, IoToken *nextToken) {
    if (self == nextToken) {
        printf("next == self!\n");
        exit(1);
    }

    if (self->nextToken) {
        IoToken_free(self->nextToken);
    }

    self->nextToken = nextToken;
}

void IoToken_print(IoToken *self) { IoToken_printSelf(self); }

/*cdoc Token IoToken_printSelf(self)
Prints the token's spelling quoted with single quotes for debug output.
Iterates name byte-by-byte rather than using printf("%s") so embedded
NULs are rendered literally. Called by IoToken_print and by
IoLexer_printTokens.
*/
void IoToken_printSelf(IoToken *self) {
    size_t i;
    printf("'");

    for (i = 0; i < self->length; i++) {
        putchar(self->name[i]);
    }

    printf("' ");
}

/*cdoc Token IoTokenType_isValidMessageName(self)
Returns true for token types that can begin a message chain: identifiers,
mono- and tri-quoted strings, and number literals. IoMessage_parser uses
this as the guard for IoMessage_parseName and IoMessage_parseNext so that
punctuation tokens never become message names.
*/
int IoTokenType_isValidMessageName(IoTokenType self) {
    switch (self) {
    case IDENTIFIER_TOKEN:
    case MONOQUOTE_TOKEN:
    case TRIQUOTE_TOKEN:
    case NUMBER_TOKEN:
    case HEXNUMBER_TOKEN:
        return 1;
    default:
        return 0;
    }
    return 0;
}
