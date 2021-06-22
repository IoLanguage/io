
// metadoc Compiler category Core
// metadoc Compiler copyright Steve Dekorte 2002
// metadoc Compiler license BSD revised
/*metadoc Compiler description
Contains methods related to the compiling code.
*/

#include "IoCompiler.h"
#include "IoNumber.h"
#include "IoMessage_parser.h"
#include "IoLexer.h"
#include "IoToken.h"
#include "IoList.h"

IoObject *IoCompiler_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"tokensForString", IoObject_tokensForString},
        //{"messageForTokens", IoObject_messageForTokens},
        {"messageForString", IoObject_messageForString},
        {"messageForString2", IoObject_messageForString2},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);
    IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("Compiler"));
    IoObject_addMethodTable_(self, methodTable);

    return self;
}

IO_METHOD(IoObject, tokensForString) {
    /*doc Compiler tokensForString(aString)
    Returns a list of token objects lexed from the input string.
    */

    IoSymbol *text = IoMessage_locals_seqArgAt_(m, locals, 0);
    IoList *tokensList = IoList_new(IOSTATE);
    IoLexer *lexer = IoLexer_new();
    IoSymbol *name = IOSYMBOL("name");
    IoSymbol *line = IOSYMBOL("line");
    IoSymbol *character = IOSYMBOL("character");
    IoSymbol *type = IOSYMBOL("type");

    IoLexer_string_(lexer, CSTRING(text));
    IoLexer_lex(lexer);

    if (IoLexer_errorToken(lexer)) {
        IoSymbol *errorString = IOSYMBOL(IoLexer_errorDescription(lexer));
        IoLexer_free(lexer);
        IoState_error_(IOSTATE, NULL, "compile error: %s",
                       CSTRING(errorString));
    } else {
        IoToken *t;

        while ((t = IoLexer_pop(lexer))) {
            IoObject *tokenObject = IoObject_new(IOSTATE);

            IoObject_setSlot_to_(tokenObject, name, IOSYMBOL(IoToken_name(t)));
            IoObject_setSlot_to_(tokenObject, line,
                                 IONUMBER(IoToken_lineNumber(t)));
            IoObject_setSlot_to_(tokenObject, character,
                                 IONUMBER(IoToken_charNumber(t)));
            IoObject_setSlot_to_(tokenObject, type,
                                 IOSYMBOL(IoToken_typeName(t)));

            IoList_rawAppend_(tokensList, tokenObject);
        }
    }

    IoLexer_free(lexer);

    return tokensList;
}

IO_METHOD(IoObject, messageForTokens) {
    /*doc Compiler messageForTokens(aList)
    Returns the compiled message object for the given token list.
    */

    return m;
}

IO_METHOD(IoObject, messageForString) {
    /*doc Compiler messageForString(aString, optionalLabelString)
    Returns the compiled message object for aString.
    */

    IoSymbol *string = IoMessage_locals_seqArgAt_(m, locals, 0);
    IoSymbol *label = IoMessage_rawLabel(m);

    if (IoMessage_argCount(m) > 1) {
        label = IoMessage_locals_symbolArgAt_(m, locals, 1);
    }

    return IoMessage_newFromText_labelSymbol_(
        IOSTATE, CSTRING((IoSymbol *)string), (IoSymbol *)label);
}

IO_METHOD(IoObject, messageForString2) {
    /*doc Compiler messageForString2(aString)
    Returns the compiled message object for aString. (Runs raw string against
    lexer directly.)
    */

    IoLexer *lexer = IoLexer_new();
    char *text = IoMessage_locals_cStringArgAt_(m, locals, 0);
    IoMessage *msg;

    IoLexer_string_(lexer, text);
    IoLexer_lex(lexer);

    msg = IoMessage_newParse(IOSTATE, lexer);

    IoLexer_free(lexer);
    return msg;
}
