/*
Copyright (c) 2003, Steve Dekorte
All rights reserved. See _BSDLicense.txt.

Aug 2004 - removed {} from op chars
- changed identifier to stop after 1 colon
*/

#include "IoLexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

//#define LEXER_DEBUG
//#define LEXER_DEBUG_TOKENS
//#define TEST_INLINE inline
#define TEST_INLINE

static IoToken *IoLexer_currentToken(IoLexer *self) {
    return List_top(self->tokenStream);
}

IoLexer *IoLexer_new(void) {
    IoLexer *self = (IoLexer *)io_calloc(1, sizeof(IoLexer));
    self->s = (char *)io_calloc(1, 1);
    self->s[0] = 0;
    self->posStack = Stack_new();
    self->tokenStack = Stack_new();
    self->tokenStream = List_new();
    self->charLineIndex = List_new();
    return self;
}

void IoLexer_free(IoLexer *self) {
    IoLexer_clear(self);
    io_free(self->s);
    Stack_free(self->posStack);
    Stack_free(self->tokenStack);
    List_free(self->tokenStream);
    List_free(self->charLineIndex);
    if (self->errorDescription)
        io_free(self->errorDescription);
    io_free(self);
}

char *IoLexer_errorDescription(IoLexer *self) {
    IoToken *et = IoLexer_errorToken(self);

    if (!self->errorDescription) {
        self->errorDescription = io_calloc(1, 1024);
        self->errorDescription[0] = 0;
    }

    if (et) {
        sprintf(self->errorDescription, "\"%s\" on line %i character %i",
                et->error, IoToken_lineNumber(et), IoToken_charNumber(et));
    }

    return self->errorDescription;
}

void IoLexer_buildLineIndex(IoLexer *self) {
    char *s = self->s;

    List_removeAll(self->charLineIndex);

    List_append_(self->charLineIndex, s);

    while (*s) {
        if (*s == '\n') {
            List_append_(self->charLineIndex, s);
        }
        s++;
    }

    List_append_(self->charLineIndex, s);
    self->lineHint = 0;
}

// next/prev character ------------------------

#define UTF8_SEQLEN(c)                                                         \
    ((c) < 0x80   ? 1                                                          \
     : (c) < 0xe0 ? 2                                                          \
     : (c) < 0xf0 ? 3                                                          \
     : (c) < 0xf8 ? 4                                                          \
     : (c) < 0xfc ? 5                                                          \
     : (c) < 0xfe ? 6                                                          \
                  : 1)
#define INVALID_CHAR 0xfffe

static uchar_t _IoLexer_DecodeUTF8(const unsigned char *s) {
    if (*s < 0x80)
        return *s;
    else if (*s < 0xc2)
        return INVALID_CHAR;
    else if (*s < 0xe0) {
        if (!((s[1] ^ 0x80) < 0x40))
            return INVALID_CHAR;
        return ((uchar_t)(s[0] & 0x1f) << 6) | (uchar_t)(s[1] ^ 0x80);
    } else if (*s < 0xf0) {
        if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40 &&
              (s[0] >= 0xe1 || s[1] >= 0xa0)))
            return INVALID_CHAR;
        return ((uchar_t)(s[0] & 0x0f) << 12) | ((uchar_t)(s[1] ^ 0x80) << 6) |
               (uchar_t)(s[2] ^ 0x80);
    } else if (*s < 0xf8) {
        if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40 &&
              (s[3] ^ 0x80) < 0x40 && (s[0] >= 0xf1 || s[1] >= 0x90)))
            return INVALID_CHAR;
        return ((uchar_t)(s[0] & 0x07) << 18) | ((uchar_t)(s[1] ^ 0x80) << 12) |
               ((uchar_t)(s[2] ^ 0x80) << 6) | (uchar_t)(s[3] ^ 0x80);
    } else if (*s < 0xfc) {
        if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40 &&
              (s[3] ^ 0x80) < 0x40 && (s[4] ^ 0x80) < 0x40 &&
              (s[0] >= 0xf9 || s[1] >= 0x88)))
            return INVALID_CHAR;
        return ((uchar_t)(s[0] & 0x03) << 24) | ((uchar_t)(s[1] ^ 0x80) << 18) |
               ((uchar_t)(s[2] ^ 0x80) << 12) | ((uchar_t)(s[3] ^ 0x80) << 6) |
               (uchar_t)(s[4] ^ 0x80);
    } else if (*s < 0xfe) {
        if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40 &&
              (s[3] ^ 0x80) < 0x40 && (s[4] ^ 0x80) < 0x40 &&
              (s[5] ^ 0x80) < 0x40 && (s[0] >= 0xfd || s[1] >= 0x84)))
            return INVALID_CHAR;
        return ((uchar_t)(s[0] & 0x01) << 30) | ((uchar_t)(s[1] ^ 0x80) << 24) |
               ((uchar_t)(s[2] ^ 0x80) << 18) | ((uchar_t)(s[3] ^ 0x80) << 12) |
               ((uchar_t)(s[4] ^ 0x80) << 6) | (uchar_t)(s[5] ^ 0x80);
    } else
        return INVALID_CHAR;
}

TEST_INLINE uchar_t IoLexer_nextChar(IoLexer *self) {
    unsigned char c = (unsigned char)*(self->current);
    int seqlen, i;
    uchar_t uch;

    if (c == 0) {
        return 0;
    } else if (c < 0x80) {
        self->current++;
        return c;
    }

    seqlen = UTF8_SEQLEN(c);

    for (i = 0; i < seqlen; i++) {
        if (self->current[i] == 0) {
            // XXX: invalid or incomplete sequence
            return 0;
        }
    }

    uch = _IoLexer_DecodeUTF8((unsigned char *)self->current);

    if (uch == INVALID_CHAR) {
        return 0;
    }

    self->current += seqlen;
    return uch;
}

TEST_INLINE uchar_t IoLexer_prevChar(IoLexer *self) {
    uchar_t uch;
    int len;

    for (len = 1; len <= 6 && self->current - len > self->s; len++) {
        unsigned char c = *(unsigned char *)(self->current - len);
        if (c < 0x80 || c >= 0xc2)
            break;
    }

    self->current -= len;
    uch = _IoLexer_DecodeUTF8((unsigned char *)self->current);
    if (uch == INVALID_CHAR)
        return 0;

    return uch;
}

TEST_INLINE char *IoLexer_current(IoLexer *self) { return self->current; }

TEST_INLINE int IoLexer_onNULL(IoLexer *self) {
    return (*(self->current) == 0);
}

// ------------------------------------------

size_t IoLexer_currentLineNumberOld(IoLexer *self) {
    size_t lineNumber = 1;
    char *s = self->s;

    while (s < self->current) {
        if (*s == '\n') {
            lineNumber++;
        }

        s++;
    }

    return lineNumber;
}

TEST_INLINE size_t IoLexer_currentLineNumber(IoLexer *self) {
    // this should be even faster than a binary search
    // since almost all results are very close to the last

    List *index = self->charLineIndex;
    size_t line = self->lineHint;
    size_t numLines = List_size(index);
    void *current = (void *)self->current;

    if (current < List_at_(index, line)) {
        // walk down lines until char is bigger than one
        while (line > 0 && !(current > List_at_(index, line))) {
            line--;
        }
        line++;
    } else {
        // walk up lines until char is less than or equal to one
        while (line < numLines && !(current <= List_at_(index, line))) {
            line++;
        }
    }

    self->lineHint = line;

    /*
    {
            size_t realLine = IoLexer_currentLineNumberOld(self);

            if (line != realLine)
            {
                    printf("mismatch on currentLine %i != %i\n", (int)line,
    (int)realLine);
            }
    }
    */
    return line;
}

void IoLexer_clear(IoLexer *self) {
    LIST_FOREACH(self->tokenStream, i, t, IoToken_free((IoToken *)t));
    List_removeAll(self->tokenStream);

    Stack_clear(self->posStack);
    Stack_clear(self->tokenStack);

    self->current = self->s;
    self->resultIndex = 0;
    self->maxChar = 0;
    self->errorToken = NULL;
}

IoToken *IoLexer_errorToken(IoLexer *self) { return self->errorToken; }

// lexing -------------------------------------

void IoLexer_string_(IoLexer *self, const char *string) {
    self->s = strcpy((char *)io_realloc(self->s, strlen(string) + 1), string);
    self->current = self->s;
    IoLexer_buildLineIndex(self);
}

void IoLexer_printLast_(IoLexer *self, int max) {
    char *s = self->s + self->maxChar;
    int i;

    for (i = 0; i < max && s[i]; i++) {
        putchar(s[i]);
    }
}

// --- token and character position stacks ---

char *IoLexer_lastPos(IoLexer *self) { return Stack_top(self->posStack); }

TEST_INLINE void IoLexer_pushPos(IoLexer *self) {
    intptr_t index = self->current - self->s;

    if (index > (intptr_t)self->maxChar) {
        self->maxChar = index;
    }

    Stack_push_(self->tokenStack,
                (void *)(intptr_t)(List_size(self->tokenStream) - 1));
    Stack_push_(self->posStack, self->current);

#ifdef LEXER_DEBUG
    printf("push: ");
    IoLexer_print(self);
#endif
}

TEST_INLINE void IoLexer_popPos(IoLexer *self) {
    Stack_pop(self->tokenStack);
    Stack_pop(self->posStack);
#ifdef LEXER_DEBUG
    printf("pop:	");
    IoLexer_print(self);
#endif
}

TEST_INLINE void IoLexer_popPosBack(IoLexer *self) {
    intptr_t i = (intptr_t)Stack_pop(self->tokenStack);
    intptr_t topIndex = (intptr_t)Stack_top(self->tokenStack);

    if (i > -1) {
        List_setSize_(self->tokenStream, i + 1);

        if (i != topIndex) // ok to io_free token
        {
            IoToken *parent = IoLexer_currentToken(self);

            if (parent) {
                IoToken_nextToken_(parent, NULL);
            }
        }
    }

    self->current = Stack_pop(self->posStack);
#ifdef LEXER_DEBUG
    printf("back: ");
    IoLexer_print(self);
#endif
}

// ------------------------------------------

int IoLexer_lex(IoLexer *self) {
    IoLexer_clear(self);
    IoLexer_pushPos(self);

    IoLexer_messageChain(self);

    if (*(self->current)) {
        // printf("Lexing error after: ");
        // IoLexer_printLast_(self, 30);
        // printf("\n");

        if (!self->errorToken) {
            if (List_size(self->tokenStream)) {
                self->errorToken = IoLexer_currentToken(self);
            } else {
                self->errorToken = IoLexer_addTokenString_length_type_(
                    self, self->current, 30, NO_TOKEN);
            }

            IoToken_error_(self->errorToken, "Syntax error near this location");
        }
        return -1;
    }
    return 0;
}

// getting results --------------------------------

IoToken *IoLexer_top(IoLexer *self) {
    return List_at_(self->tokenStream, self->resultIndex);
}

IoTokenType IoLexer_topType(IoLexer *self) {
    if (!IoLexer_top(self)) {
        return 0;
    }

    return IoLexer_top(self)->type;
}

IoToken *IoLexer_pop(IoLexer *self) {
    IoToken *t = IoLexer_top(self);
    self->resultIndex++;
    return t;
}

// stack management --------------------------------

void IoLexer_print(IoLexer *self) {
    IoToken *first = List_first(self->tokenStream);

    if (first) {
        IoToken_print(first);
    }

    printf("\n");
}

void IoLexer_printTokens(IoLexer *self) {
    int i;

    for (i = 0; i < List_size(self->tokenStream); i++) {
        IoToken *t = List_at_(self->tokenStream, i);

        printf("'%s'", t->name);
        printf(" %s ", IoToken_typeName(t));

        if (i < List_size(self->tokenStream) - 1) {
            printf(", ");
        }
    }

    printf("\n");
}

// grabbing ---------------------------------------------

int IoLexer_grabLength(IoLexer *self) {
    char *s1 = IoLexer_lastPos(self);
    char *s2 = IoLexer_current(self);

    return (int)(s2 - s1);
}

void IoLexer_grabTokenType_(IoLexer *self, IoTokenType type) {
    char *s1 = IoLexer_lastPos(self);
    char *s2 = IoLexer_current(self);
    size_t len = (s2 - s1);

    if (!len) {
        printf("IoLexer fatal error: empty token\n");
        exit(1);
    }

    IoLexer_addTokenString_length_type_(self, s1, len, type);
}

IoToken *IoLexer_addTokenString_length_type_(IoLexer *self, const char *s1,
                                             size_t len, IoTokenType type) {
    IoToken *top = IoLexer_currentToken(self);
    IoToken *t = IoToken_new();

    t->lineNumber = (int)IoLexer_currentLineNumber(self);
    // t->charNumber = (int)(s1 - self->s);
    t->charNumber = (int)(self->current - self->s);

    if (t->charNumber < 0) {
        printf("bad t->charNumber = %i\n", t->charNumber);
    }

    IoToken_name_length_(t, s1, len);
    IoToken_type_(t, type);

    if (top) {
        IoToken_nextToken_(top, t);
    }

    List_push_(self->tokenStream, t);
#ifdef LEXER_DEBUG_TOKENS
    printf("token '%s' %s\n", t->name, IoToken_typeName(t));
#endif

    return t;
}

// reading ------------------------------------

void IoLexer_messageChain(IoLexer *self) {
    do {
        while (IoLexer_readTerminator(self) || IoLexer_readSeparator(self) ||
               IoLexer_readComment(self)) {
        }
    } while (IoLexer_readMessage(self));
}

// message -------------------------------

static void IoLexer_readMessage_error(IoLexer *self, const char *name) {
    IoLexer_popPosBack(self);
    self->errorToken = IoLexer_currentToken(self);
    IoToken_error_(self->errorToken, name);
}

int IoLexer_readTokenChars_type_(IoLexer *self, const char *chars,
                                 IoTokenType type) {
    while (*chars) {
        if (IoLexer_readTokenChar_type_(self, *chars, type))
            return 1;
        chars++;
    }

    return 0;
}

const char *IoLexer_nameForGroupChar_(IoLexer *self, char groupChar) {
    switch (groupChar) {
    case '(':
        return "";
    case '[':
        return "squareBrackets";
    case '{':
        return "curlyBrackets";
    }

    printf("IoLexer: fatal error - invalid group char %c\n", groupChar);
    exit(1);
}

// static char *specialChars = ":._";
static char *specialChars = "._";

int IoLexer_readMessage(IoLexer *self) {
    char foundSymbol;

    IoLexer_pushPos(self);
    IoLexer_readPadding(self);

    foundSymbol = IoLexer_readSymbol(self);

    {
        char groupChar;
        while (IoLexer_readSeparator(self) || IoLexer_readComment(self)) {
        }

        groupChar = *IoLexer_current(self);

        if (groupChar &&
            (strchr("[{", groupChar) || (!foundSymbol && groupChar == '('))) {
            char *groupName =
                (char *)IoLexer_nameForGroupChar_(self, groupChar);
            IoLexer_addTokenString_length_type_(
                self, groupName, strlen(groupName), IDENTIFIER_TOKEN);
        }

        if (IoLexer_readTokenChars_type_(self, "([{", OPENPAREN_TOKEN)) {
            IoLexer_readPadding(self);
            do {
                IoTokenType type = IoLexer_currentToken(self)->type;

                IoLexer_readPadding(self);
                // Empty argument: (... ,)
                if (COMMA_TOKEN == type) {
                    char c = *IoLexer_current(self);

                    if (',' == c || strchr(")]}", c)) {
                        IoLexer_readMessage_error(
                            self, "missing argument in argument list");
                        return 0;
                    }
                }

                // if (groupChar == '[') specialChars = "._";
                IoLexer_messageChain(self);
                // if (groupChar == '[') specialChars = ":._";
                IoLexer_readPadding(self);

            } while (IoLexer_readTokenChar_type_(self, ',', COMMA_TOKEN));

            if (!IoLexer_readTokenChars_type_(self, ")]}", CLOSEPAREN_TOKEN)) {
                /*
                char c = *IoLexer_current(self);

                if (strchr("([{", c))
                {
                        IoLexer_readMessage_error(self, "expected a message but
                instead found a open group character");
                }
                else
                {
                        IoLexer_readMessage_error(self, "missing closing group
                character for argument list");
                }
                */
                if (groupChar == '(') {
                    IoLexer_readMessage_error(self, "unmatched ()s");
                } else if (groupChar == '[') {
                    IoLexer_readMessage_error(self, "unmatched []s");
                } else if (groupChar == '{') {
                    IoLexer_readMessage_error(self, "unmatched {}s");
                }
                // printf("Token %p error: %s - %s\n", t, t->error,
                // IoToken_error(t));
                return 0;
            }

            IoLexer_popPos(self);
            return 1;
        }

        if (foundSymbol) {
            IoLexer_popPos(self);
            return 1;
        }
    }
    IoLexer_popPosBack(self);
    return 0;
}

int IoLexer_readPadding(IoLexer *self) {
    int r = 0;

    while (IoLexer_readWhitespace(self) || IoLexer_readComment(self)) {
        r = 1;
    }

    return r;
}

// symbols ------------------------------------------

int IoLexer_readSymbol(IoLexer *self) {
    if (IoLexer_readNumber(self) || IoLexer_readOperator(self) ||
        IoLexer_readIdentifier(self) || IoLexer_readQuote(self))
        return 1;
    return 0;
}

int IoLexer_readIdentifier(IoLexer *self) {
    IoLexer_pushPos(self);

    while (IoLexer_readLetter(self) || IoLexer_readDigit(self) ||
           IoLexer_readSpecialChar(self)) {
    }

    if (IoLexer_grabLength(self)) {
        // avoid grabing : on last character if followed by =

        /*
                        char *current = IoLexer_current(self);

                        if (*(current - 1) == ':' && *current == '=')
                        {
                                IoLexer_prevChar(self);
                        }
                        */

        IoLexer_grabTokenType_(self, IDENTIFIER_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);

    return 0;
}

int IoLexer_readOperator(IoLexer *self) {
    uchar_t c;
    IoLexer_pushPos(self);
    // ok if first character is a colon
    c = IoLexer_nextChar(self);
    // printf("IoLexer_nextChar(self) = %c %i\n", c, c);

    if (c == 0) {
        IoLexer_popPosBack(self);
        return 0;
    } else {
        IoLexer_prevChar(self);
    }
    /*
    if (c != ':')
    {
            IoLexer_prevChar(self);
    }
    */

    while (IoLexer_readOpChar(self)) {
    }

    if (IoLexer_grabLength(self)) {
        IoLexer_grabTokenType_(self, IDENTIFIER_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}

// comments ------------------------------------------

int IoLexer_readComment(IoLexer *self) {
    return (IoLexer_readSlashStarComment(self) ||
            IoLexer_readSlashSlashComment(self) ||
            IoLexer_readPoundComment(self));
}

int IoLexer_readSlashStarComment(IoLexer *self) {
    IoLexer_pushPos(self);

    if (IoLexer_readString_(self, "/*")) {
        unsigned int nesting = 1;

        while (nesting > 0) {
            if (IoLexer_readString_(self, "/*")) {
                IoLexer_nextChar(self);
                nesting++;
            } else if (IoLexer_readString_(self, "*/")) {
                // otherwise we end up trimming the last char
                if (nesting > 1)
                    IoLexer_nextChar(self);
                nesting--;
            } else {
                uchar_t c = IoLexer_nextChar(self);
                if (c == 0) {
                    self->errorToken = IoLexer_currentToken(self);

                    if (!self->errorToken) {
                        IoLexer_grabTokenType_(self, NO_TOKEN);
                        self->errorToken = IoLexer_currentToken(self);
                    }

                    if (self->errorToken) {
                        IoToken_error_(self->errorToken,
                                       "unterminated comment");
                    }

                    IoLexer_popPosBack(self);
                    return 0;
                }
            }
        }
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}

int IoLexer_readSlashSlashComment(IoLexer *self) {
    IoLexer_pushPos(self);

    if (IoLexer_nextChar(self) == '/') {
        if (IoLexer_nextChar(self) == '/') {
            while (IoLexer_readNonReturn(self)) {
            }
            // IoLexer_grabTokenType_(self, COMMENT_TOKEN);
            IoLexer_popPos(self);
            return 1;
        }
    }

    IoLexer_popPosBack(self);
    return 0;
}

int IoLexer_readPoundComment(IoLexer *self) {
    IoLexer_pushPos(self);

    if (IoLexer_nextChar(self) == '#') {
        while (IoLexer_readNonReturn(self)) {
        }
        // IoLexer_grabTokenType_(self, COMMENT_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}

// quotes -----------------------------------------

int IoLexer_readQuote(IoLexer *self) {
    return (IoLexer_readTriQuote(self) || IoLexer_readMonoQuote(self));
}

int IoLexer_readMonoQuote(IoLexer *self) {
    IoLexer_pushPos(self);

    if (IoLexer_nextChar(self) == '"') {
        for (;;) {
            uchar_t c = IoLexer_nextChar(self);

            if (c == '"') {
                break;
            }

            if (c == '\\') {
                IoLexer_nextChar(self);
                continue;
            }

            if (c == 0) {
                self->errorToken = IoLexer_currentToken(self);

                if (self->errorToken) {
                    IoToken_error_(self->errorToken, "unterminated quote");
                }

                IoLexer_popPosBack(self);
                return 0;
            }
        }

        IoLexer_grabTokenType_(self, MONOQUOTE_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}

int IoLexer_readTriQuote(IoLexer *self) {
    IoLexer_pushPos(self);

    if (IoLexer_readString_(self, "\"\"\"")) {
        while (!IoLexer_readString_(self, "\"\"\"")) {
            uchar_t c = IoLexer_nextChar(self);

            if (c == 0) {
                IoLexer_popPosBack(self);
                return 0;
            }
        }

        IoLexer_grabTokenType_(self, TRIQUOTE_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}

// helpers ----------------------------

int IoLexer_readTokenChar_type_(IoLexer *self, char c, IoTokenType type) {
    IoLexer_pushPos(self);

    if (IoLexer_readChar_(self, c)) {
        IoLexer_grabTokenType_(self, type);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}

int IoLexer_readTokenString_(IoLexer *self, const char *s) {
    IoLexer_pushPos(self);

    if (IoLexer_readString_(self, s)) {
        IoLexer_grabTokenType_(self, IDENTIFIER_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}

int IoLexer_readString_(IoLexer *self, const char *s) {
    size_t len = strlen(s);

    if (IoLexer_onNULL(self)) {
        return 0;
    }

    if (strncmp(self->current, s, len) == 0) {
        self->current += len;
        return 1;
    }

    return 0;
}

TEST_INLINE int IoLexer_readCharIn_(IoLexer *self, const char *s) {
    if (!IoLexer_onNULL(self)) {
        uchar_t c = IoLexer_nextChar(self);

        if (c < 0x80 && strchr(s, c)) {
            return 1;
        }

        IoLexer_prevChar(self);
    }
    return 0;
}

TEST_INLINE int IoLexer_readCharInRange_(IoLexer *self, uchar_t first,
                                         uchar_t last) {
    if (!IoLexer_onNULL(self)) {
        uchar_t c = IoLexer_nextChar(self);

        if (c >= first && c <= last) {
            return 1;
        }

        IoLexer_prevChar(self);
    }
    return 0;
}

int IoLexer_readChar_(IoLexer *self, char c) {
    if (!IoLexer_onNULL(self)) {
        uchar_t nc = IoLexer_nextChar(self);

        if (nc && nc == c) {
            return 1;
        }

        IoLexer_prevChar(self);
    }
    return 0;
}

int IoLexer_readCharAnyCase_(IoLexer *self, char c) {
    if (!IoLexer_onNULL(self)) {
        uchar_t nc = IoLexer_nextChar(self);

        if (nc && tolower(nc) == tolower(c)) {
            return 1;
        }

        IoLexer_prevChar(self);
    }
    return 0;
}

int IoLexer_readNonASCIIChar_(IoLexer *self) {
    if (!IoLexer_onNULL(self)) {
        uchar_t nc = IoLexer_nextChar(self);

        if (nc >= 0x80)
            return 1;

        IoLexer_prevChar(self);
    }
    return 0;
}

int IoLexer_readNonReturn(IoLexer *self) {
    if (IoLexer_onNULL(self))
        return 0;
    if (IoLexer_nextChar(self) != '\n')
        return 1;
    IoLexer_prevChar(self);
    return 0;
}

int IoLexer_readNonQuote(IoLexer *self) {
    if (IoLexer_onNULL(self))
        return 0;
    if (IoLexer_nextChar(self) != '"')
        return 1;
    IoLexer_prevChar(self);
    return 0;
}

// character definitions ----------------------------

int IoLexer_readCharacters(IoLexer *self) {
    int read = 0;

    while (IoLexer_readCharacter(self)) {
        read = 1;
    }

    return read;
}

int IoLexer_readCharacter(IoLexer *self) {
    return (IoLexer_readLetter(self) || IoLexer_readDigit(self) ||
            IoLexer_readSpecialChar(self) || IoLexer_readOpChar(self));
}

int IoLexer_readOpChar(IoLexer *self) {
    // return IoLexer_readCharIn_(self, ":'~!@$%^&*-+=|\\<>?/");
    return IoLexer_readCharIn_(self, ":'~!@$%^&*-+=|\\<>?/");
}

int IoLexer_readSpecialChar(IoLexer *self) {
    return IoLexer_readCharIn_(self, specialChars);
}

int IoLexer_readDigit(IoLexer *self) {
    return IoLexer_readCharInRange_(self, '0', '9');
}

int IoLexer_readLetter(IoLexer *self) {
    return IoLexer_readCharInRange_(self, 'A', 'Z') ||
           IoLexer_readCharInRange_(self, 'a', 'z') ||
           IoLexer_readCharIn_(self, ":") || IoLexer_readNonASCIIChar_(self);
}

// terminator -------------------------------

int IoLexer_readTerminator(IoLexer *self) {
    int terminated = 0;
    IoLexer_pushPos(self);
    IoLexer_readSeparator(self);

    while (IoLexer_readTerminatorChar(self)) {
        terminated = 1;
        IoLexer_readSeparator(self);
    }

    if (terminated) {
        IoToken *top = IoLexer_currentToken(self);

        // avoid double terminators
        if (top && IoToken_type(top) == TERMINATOR_TOKEN) {
            return 1;
        }

        IoLexer_addTokenString_length_type_(self, ";", 1, TERMINATOR_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}

int IoLexer_readTerminatorChar(IoLexer *self) {
    return IoLexer_readCharIn_(self, ";\n");
}

// separator --------------------------------

int IoLexer_readSeparator(IoLexer *self) {
    IoLexer_pushPos(self);

    while (IoLexer_readSeparatorChar(self)) {
    }

    if (IoLexer_grabLength(self)) {
        // IoLexer_grabTokenType_(self, SEPERATOR_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}

int IoLexer_readSeparatorChar(IoLexer *self) {
    if (IoLexer_readCharIn_(self, " \f\r\t\v")) {
        return 1;
    } else {
        IoLexer_pushPos(self);
        if (IoLexer_readCharIn_(self, "\\")) {
            while (IoLexer_readCharIn_(self, " \f\r\t\v")) {
            }

            if (IoLexer_readCharIn_(self, "\n")) {
                IoLexer_popPos(self);
                return 1;
            }
        }
        IoLexer_popPosBack(self);
        return 0;
    }
}

// whitespace -----------------------------------

int IoLexer_readWhitespace(IoLexer *self) {
    IoLexer_pushPos(self);

    while (IoLexer_readWhitespaceChar(self)) {
    }

    if (IoLexer_grabLength(self)) {
        // IoLexer_grabTokenType_(self, WHITESPACE_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}

int IoLexer_readWhitespaceChar(IoLexer *self) {
    return IoLexer_readCharIn_(self, " \f\r\t\v\n");
}

int IoLexer_readDigits(IoLexer *self) {
    int read = 0;

    IoLexer_pushPos(self);

    while (IoLexer_readDigit(self)) {
        read = 1;
    }

    if (!read) {
        IoLexer_popPosBack(self);
        return 0;
    }

    IoLexer_popPos(self);
    return read;
}

int IoLexer_readNumber(IoLexer *self) {
    return (IoLexer_readHexNumber(self) || IoLexer_readDecimal(self));
}

int IoLexer_readExponent(IoLexer *self) {
    if (IoLexer_readCharAnyCase_(self, 'e')) {
        if (!IoLexer_readChar_(self, '-')) {
            IoLexer_readChar_(self, '+');
        }

        if (!IoLexer_readDigits(self)) {
            return -1;
        }

        return 1;
    }
    return 0;
}

int IoLexer_readDecimalPlaces(IoLexer *self) {
    if (IoLexer_readChar_(self, '.')) {
        if (!IoLexer_readDigits(self)) {
            return -1;
        }

        return 1;
    }
    return 0;
}

int IoLexer_readDecimal(IoLexer *self) {
    IoLexer_pushPos(self);

    if (IoLexer_readDigits(self)) {
        if (IoLexer_readDecimalPlaces(self) == -1) {
            goto error;
        }
    } else {
        if (IoLexer_readDecimalPlaces(self) != 1) {
            goto error;
        }
    }

    if (IoLexer_readExponent(self) == -1) {
        goto error;
    }

    if (IoLexer_grabLength(self)) {
        IoLexer_grabTokenType_(self, NUMBER_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }
error:
    IoLexer_popPosBack(self);
    return 0;
}

int IoLexer_readHexNumber(IoLexer *self) {
    int read = 0;

    IoLexer_pushPos(self);

    if (IoLexer_readChar_(self, '0') && IoLexer_readCharAnyCase_(self, 'x')) {
        while (IoLexer_readDigits(self) || IoLexer_readCharacters(self)) {
            read++;
        }
    }

    if (read && IoLexer_grabLength(self)) {
        IoLexer_grabTokenType_(self, HEXNUMBER_TOKEN);
        IoLexer_popPos(self);
        return 1;
    }

    IoLexer_popPosBack(self);
    return 0;
}
