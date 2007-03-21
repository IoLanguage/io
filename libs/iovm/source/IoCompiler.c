/*#io
Compiler ioDoc(
			docCopyright("Steve Dekorte", 2002)
			docLicense("BSD revised")
			docDescription("Contains methods related to the compiling code.")
			docCategory("Core")
			*/

#include "IoCompiler.h"
#include "IoNumber.h"
#include "IoMessage_parser.h"
#include "IoLexer.h"
#include "IoToken.h"
#include "IoList.h"

IoObject *IoCompiler_proto(void *state)
{
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

IoObject *IoObject_tokensForString(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("tokensForString(aString)", 
		   "Returns a list of token objects lexed from the input string.")
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
	
	if (IoLexer_errorToken(lexer))
	{ 
		IoSymbol *errorString  = IOSYMBOL(IoLexer_errorDescription(lexer));
		IoLexer_free(lexer);
		IoState_error_(IOSTATE, NULL, "compile error: %s", CSTRING(errorString));
	}
	else
	{
		IoToken *t;
		
		while ((t = IoLexer_pop(lexer)))
		{
			IoObject *tokenObject = IoObject_new(IOSTATE);
			
			IoObject_setSlot_to_(tokenObject, name, IOSYMBOL(IoToken_name(t)));
			IoObject_setSlot_to_(tokenObject, line, IONUMBER(IoToken_lineNumber(t)));
			IoObject_setSlot_to_(tokenObject, character, IONUMBER(IoToken_charNumber(t)));
			IoObject_setSlot_to_(tokenObject, type, IOSYMBOL(IoToken_typeName(t)));
			
			IoList_rawAppend_(tokensList, tokenObject);
		}
	}
	
	IoLexer_free(lexer);
	
	return tokensList;
}

IoObject *IoObject_messageForTokens(IoObject *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("messageForTokens(aList)", 
		   "Returns the compiled message object for the given token list.")
	*/
	
	return m;
}

IoObject *IoObject_messageForString(IoObject *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("messageForString(aString, optionalLabelString)", 
		   "Returns the compiled message object for aString.")
	*/
	
	IoSymbol *string = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSymbol *label  = IoMessage_rawLabel(m);
	IoObject *result = IONIL(self);
	
	if (IoMessage_argCount(m) > 1) 
	{ 
		label = IoMessage_locals_symbolArgAt_(m, locals, 1); 
	}
	
	//printf("CSTRING((IoSymbol *)string) = %s\n", CSTRING((IoSymbol *)string));
	
	result = IoMessage_newFromText_label_(IOSTATE,
								   CSTRING((IoSymbol *)string), 
								   CSTRING((IoSymbol *)label));
	
	/*
	 {
		 IoState_print_(IOSTATE, "parsed: "); 
		 IoMessage_print((IoMessage *)result); 
		 IoState_print_(IOSTATE, "\n"); 
	 }
	 */
	
	return (IoObject *)result;
}


IoObject *IoObject_messageForString2(IoObject *self, IoObject *locals, IoMessage *m)
{ 
	IoLexer *lexer = IoLexer_new();
	char *text = IoMessage_locals_cStringArgAt_(m, locals, 0);
	IoMessage *msg;
	
	IoLexer_string_(lexer, text);
	IoLexer_lex(lexer);

	msg = IoMessage_newParse(IOSTATE, lexer);

	IoLexer_free(lexer);
	return msg;
}
