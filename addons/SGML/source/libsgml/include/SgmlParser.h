/*
 * libsgml -- SGML state machine parsing library.
 *                                                                  
 * Copyright (c) 2002 Uninformed Research (http://www.uninformed.org)
 * All rights reserved.
 *
 * skape
 * mmiller@hick.org
 */
/**
 * @ingroup SGMLParser
 */
/**
 * @{
 */
/**
 * @example test.c
 * Example for using user defined callbacks for parsing SGML.
 */
#ifndef _LIBHTTP_SGMLPARSER_H
#define _LIBHTTP_SGMLPARSER_H

#ifdef __cplusplus
extern "C" {
#endif

enum SgmlExtensionType {
	SGML_EXTENSION_TYPE_XML = 0,
	SGML_EXTENSION_TYPE_HTML,

	SGML_EXTENSION_TYPE_CUSTOM = 255
};

struct _sgml_parser;

/**
 * 	Basic handlers of SGML parsing.
 *
 * @short Basic handlers for SGML parsing.
 */
typedef struct _sgml_handlers {

	/**
	 * Called prior to parsing the document
	 */
	void (*preparse)(struct _sgml_parser *parser, void *userContext);
	/**
	 * Called after parsing the document.
	 */
	void (*postparse)(struct _sgml_parser *parser, void *userContext);

	/**
	 * Called when an element is beginning.
	 */
	void (*elementBegin)(struct _sgml_parser *parser, void *userContext, const char *elementName);
	/**
	 * Called when an element is closed.
	 */
	void (*elementEnd)(struct _sgml_parser *parser, void *userContext, const char *elementName);

	/**
	 * Called when an attribute is done.
	 */
	void (*attributeNew)(struct _sgml_parser *parser, void *userContext, const char *attributeName, const char *attributeValue);

	/**
	 * Called when a text node is done.
	 */
	void (*textNew)(struct _sgml_parser *parser, void *userContext, const char *text);

	/**
	 * Called when a comment is done.
	 */
	void (*commentNew)(struct _sgml_parser *parser, void *userContext, const char *comment);

} SGML_HANDLERS;

/**
 * @}
 */

#define SGML_STC_LETTER_TYPE_SPECIFIC   0x00
#define SGML_STC_LETTER_TYPE_SPECIFICWS 0x01
#define SGML_STC_LETTER_TYPE_NOT        0x02
#define SGML_STC_LETTER_TYPE_NOTWS      0x03
#define SGML_STC_LETTER_TYPE_ANY        0x04

#define SGML_STC_FLAG_DIVERT           (1 << 0)
#define SGML_STC_FLAG_UPDATE_STATE     (1 << 1)
#define SGML_STC_FLAG_INCL_IN_BUFFER   (1 << 2)

#define SGML_PARSER_STATE_INTEXT                      (1 << 0)

#define SGML_PARSER_STATE_INELEMENT                   (1 << 1)
#define SGML_PARSER_STATE_INELEMENTNAME               (1 << 2)
#define SGML_PARSER_STATE_INELEMENTNAME_ACTUAL        (1 << 3)
#define SGML_PARSER_STATE_INELEMENTCLOSURE            (1 << 4)
	
#define SGML_PARSER_STATE_INATTRIBUTENAME             (1 << 5)
#define SGML_PARSER_STATE_INATTRIBUTENAME_ACTUAL      (1 << 6)

#define SGML_PARSER_STATE_INATTRIBUTEVALUE_NS         (1 << 10)

#define SGML_PARSER_STATE_INATTRIBUTEVALUE_S          (1 << 12) 

#define SGML_PARSER_STATE_INCOMMENT                   (1 << 15)
#define SGML_PARSER_STATE_INCOMMENTGOTEXCLAMATION     (1 << 16)
#define SGML_PARSER_STATE_INCOMMENTGOTDASH1           (1 << 17)
#define SGML_PARSER_STATE_INCOMMENTGOTDASH2           (1 << 18)

#define SGML_PARSER_STATE_INATTRIBUTEVALUE_DBLQUOTE   (1 << 19)

typedef struct _sgml_state_table_rule {

	unsigned long stateIndexId;

	unsigned char letterType;
	unsigned char letter;

	unsigned long flags;

	unsigned long divertTableId;

	unsigned long isState;
	unsigned long notState;

	unsigned long addState;	
	unsigned long remState;

} SGML_STATE_TABLE_RULE;

typedef struct _sgml_state_table {

	unsigned long          stateIndexId;

	SGML_STATE_TABLE_RULE  *rules;
	unsigned long          ruleSize;

} SGML_STATE_TABLE;

typedef struct _sgml_parser {

	enum SgmlExtensionType type;

	SGML_HANDLERS          handlers;	

	SGML_STATE_TABLE       *stateTable;
	unsigned long          stateTableElements;
	SGML_STATE_TABLE_RULE  *stateTableRules;
	unsigned long          stateTableRuleElements;

	struct {

		SGML_STATE_TABLE   *currentState;

		char               *lastElementName;
		char               *lastAttributeName;

		char               *currentBuffer;
		unsigned long      currentBufferSize;

		unsigned long      state;

		void               *extensionContext;
		void               *userContext;

		void               (*onStateChange)(struct _sgml_parser *parser, unsigned long oldState, unsigned long newState);
		void               (*onDivert)(struct _sgml_parser *parser, unsigned long newIndex, unsigned long oldState, unsigned long newState, const char *lastBuffer, unsigned long lastBufferSize);

		void               (*setExtensionParam)(struct _sgml_parser *parser, void *extensionContext, unsigned long param, void *value);
		void               (*getExtensionParam)(struct _sgml_parser *parser, void *extensionContext, unsigned long param, void *value);

	} internal;

} SGML_PARSER;

/**
 * @addtogroup SGMLParser
 *
 * @{
 */

/**
 * Constructs and initializes a new SGML_PARSER instance.
 *
 * @return On success, an initialized SGML_PARSER instance is returned.  Otherwise, NULL is returned.
 */
SGML_PARSER *sgmlParserNew();
/**
 * Initializes an SGML_PARSER.
 *
 * The type parameter can be one of the following:
 *
 * @li SGML_EXTENSION_TYPE_XML
 * 		Use the XML parser.
 * @li SGML_EXTENSION_TYPE_HTML
 * 		Use the HTML parser.
 * @li SGML_EXTENSION_TYPE_CUSTOM
 * 		Use a custom parser.
 *
 * @param  parser      [in] The parser context.
 * @param  type        [in] The type of parser to use.
 * @param  handlers    [in] User defined handlers.  These should only be set if the type is SGML_EXTENSION_TYPE_CUSTOM.
 * @param  userContext [in] The arbitrary user context that is passed into the handlers.
 * @return On success, one is returned.
 */
unsigned long sgmlParserInitialize(SGML_PARSER *parser, enum SgmlExtensionType type, SGML_HANDLERS *handlers, void *userContext);
/**
 * Destroys and deinitializes an SGML_PARSER.  If the destroyParser parameter is 1, the pointer passed in is deallocated by free().
 *
 * @param  parser        [in] The parser context.
 * @param  destroyParser [in] 1 if the parser should be destroyed by free(), 0 if not.
 */
void sgmlParserDestroy(SGML_PARSER *parser, unsigned char destroyParser);

/**
 * Parses a given string.
 *
 * @param  parser       [in] The parser context.
 * @param  string       [in] The string to parser.
 * @param  stringLength [in] The length of the string to be parsed.
 * @return On success, one is returned.  Otherwise, 0 is returned.
 */
unsigned long sgmlParserParseString(SGML_PARSER *parser, const char *string, const unsigned long stringLength);
/**
 * Parser a given file.
 *
 * @param  parser [in] The parser context.
 * @param  file   [in] The name of the file to parse.
 * @return On success, one is returned.  Otherwise, 0 is returned.
 */
unsigned long sgmlParserParseFile(SGML_PARSER *parser, const char *file);

/**
 * Sets a parameter on a given SGML parser extension.
 *
 * If the type passed to initialize was...
 *
 * @li SGML_PARSER_TYPE_XML
 * 		Look at sgmlExtensionXmlSetParam
 * @li SGML_PARSER_TYPE_HTML
 * 		Look at sgmlExtensionHtmlSetParam
 * @li SGML_PARSER_TYPE_CUSTOM
 * 		This method has no use.
 */
void sgmlParserExtensionSetParam(SGML_PARSER *parser, unsigned long param, void *value);
/**
 * Gets a parameter on a given SGML parser extension.
 *
 * If the type passed to initialize was...
 *
 * @li SGML_PARSER_TYPE_XML
 * 		Look at sgmlExtensionXmlSetParam
 * @li SGML_PARSER_TYPE_HTML
 * 		Look at sgmlExtensionHtmlSetParam
 * @li SGML_PARSER_TYPE_CUSTOM
 * 		This method has no use.
 */
void sgmlParserExtensionGetParam(SGML_PARSER *parser, unsigned long param, void *value);

#define sgmlParserGetExtensionContext(parser) parser->internal.extensionContext
#define sgmlParserGetUserContext(parser) parser->internal.userContext

/**
 * @}
 */

void _sgmlParserInitializeStateTable(SGML_PARSER *parser);
void _sgmlParserInitializeStateTableRules(SGML_PARSER *parser);

unsigned long _sgmlParseChunk(SGML_PARSER *parser, const char *chunk, const unsigned long chunkSize);
void _sgmlParserAppendBuffer(SGML_PARSER *parser, const char *chunk, unsigned long startOffset, unsigned long length);
void _sgmlParserResetBuffer(SGML_PARSER *parser);

void _sgmlOnStateChange(SGML_PARSER *parser, unsigned long oldState, unsigned long newState);
void _sgmlOnDivert(SGML_PARSER *parser, unsigned long newIndex, unsigned long oldState, unsigned long newState, const char *lastBuffer, unsigned long lastBufferSize);

#ifdef __cplusplus
}
#endif

#endif
