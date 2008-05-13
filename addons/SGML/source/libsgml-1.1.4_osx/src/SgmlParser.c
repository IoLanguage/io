/*
 * libsgml -- SGML state machine parsing library.
 *                                                                  
 * Copyright (c) 2002 Uninformed Research (http://www.uninformed.org)
 * All rights reserved.
 *
 * skape
 * mmiller@hick.org
 */
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "SgmlParser.h"
#include "SgmlExtensionHtml.h"
#include "SgmlExtensionXml.h"

#define SGML_PARSER_STATEINDEX_INTEXT                 0x0000
#define SGML_PARSER_STATEINDEX_INELEMENTNAME          0x0001
#define SGML_PARSER_STATEINDEX_INATTRIBUTENAME        0x0002
#define SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_NS    0x0003
#define SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_S     0x0004
#define SGML_PARSER_STATEINDEX_INCOMMENT              0x0005

/*
 * All the possible logical states for an SGML document.
 */
SGML_STATE_TABLE sgmlStateTable[] = {
	{	// Text
		SGML_PARSER_STATEINDEX_INTEXT,
		NULL,
		0
	},
	{	// ElementName
		SGML_PARSER_STATEINDEX_INELEMENTNAME,
		NULL,
		0
	},
	{	// AttributeName
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		NULL,
		0
	},
	{	// AttributeValue_NoSpaces
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_NS,
		NULL,
		0
	},
	{	// AttributeValue_Spaces
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_S,
		NULL,
		0
	},
	{	// Comment
		SGML_PARSER_STATEINDEX_INCOMMENT,
		NULL,
		0
	},	
};

SGML_STATE_TABLE_RULE sgmlStateTableRules[] = {
	/*
	 * SGML_PARSER_STATEINDEX_INTEXT
	 * Text mode rules
	 *
	 * Specific changes:
	 *		<					(divert SGML_PARSER_STATEINDEX_INELEMENTNAME)
	 */
	{
		SGML_PARSER_STATEINDEX_INTEXT,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'<',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INELEMENTNAME,
		0,
		0,
		SGML_PARSER_STATE_INELEMENT | SGML_PARSER_STATE_INELEMENTNAME,
		SGML_PARSER_STATE_INTEXT
	},
	/*
	 * SGML_PARSER_STATEINDEX_INELEMENTNAME
	 * ElementName mode rules
	 *
	 * Specific changes:
	 *		/         (flag as closure element)
	 *		>         (divert SGML_PARSER_STATEIDNEX_INTEXT)
	 *    !         (if not actual, update state setting SGML_PARSER_STATE_INCOMMENTEXCLAMATION)
	 *    -         (if state EXCLA, update state setting SGML_PARSER_STATE_INCOMMENTDASH1)
	 *    -         (if state DASH1, divert SGML_PARSER_STATEINDEX_INCOMMENT)
	 *		not ' '   (if not actual, start with actual name of element)
	 *		' '       (if actual, divert SGML_PARSER_STATE_INATTRIBUTENAME)
	 */
	{	// If we see a / in the name anywhere, flag this is as a closure element.
		SGML_PARSER_STATEINDEX_INELEMENTNAME,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'/',
		SGML_STC_FLAG_UPDATE_STATE | SGML_STC_FLAG_INCL_IN_BUFFER,
		0,
		0,
		SGML_PARSER_STATE_INELEMENTCLOSURE,
		SGML_PARSER_STATE_INELEMENTCLOSURE,
		0
	},
	{	// If we hit a close element tag, we've reached the end, divert back to text mode.
		SGML_PARSER_STATEINDEX_INELEMENTNAME,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'>',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INTEXT,
		0,
		0,
		SGML_PARSER_STATE_INTEXT,
		SGML_PARSER_STATE_INELEMENTNAME | SGML_PARSER_STATE_INELEMENTNAME_ACTUAL | SGML_PARSER_STATE_INELEMENT | SGML_PARSER_STATE_INELEMENTCLOSURE | SGML_PARSER_STATE_INCOMMENTGOTEXCLAMATION | SGML_PARSER_STATE_INCOMMENTGOTDASH1
	},
	{
		SGML_PARSER_STATEINDEX_INELEMENTNAME,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'!',
		SGML_STC_FLAG_UPDATE_STATE | SGML_STC_FLAG_INCL_IN_BUFFER,
		0,
		0,
		SGML_PARSER_STATE_INELEMENTNAME_ACTUAL,
		SGML_PARSER_STATE_INCOMMENTGOTEXCLAMATION,
		0
	},
	{
		SGML_PARSER_STATEINDEX_INELEMENTNAME,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'-',
		SGML_STC_FLAG_UPDATE_STATE | SGML_STC_FLAG_INCL_IN_BUFFER,
		0,
		SGML_PARSER_STATE_INCOMMENTGOTEXCLAMATION,
		SGML_PARSER_STATE_INCOMMENTGOTDASH1,
		SGML_PARSER_STATE_INCOMMENTGOTDASH1,
		0
	},
	{
		SGML_PARSER_STATEINDEX_INELEMENTNAME,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'-',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INCOMMENT,
		SGML_PARSER_STATE_INCOMMENTGOTDASH1,
		0,
		SGML_PARSER_STATE_INCOMMENT,
		SGML_PARSER_STATE_INELEMENTNAME | SGML_PARSER_STATE_INELEMENTNAME_ACTUAL | SGML_PARSER_STATE_INELEMENTCLOSURE | SGML_PARSER_STATE_INCOMMENTGOTEXCLAMATION | SGML_PARSER_STATE_INCOMMENTGOTDASH1 
	},
	{	// If we hit another space after the name, now we divert to the attribute name section.
		SGML_PARSER_STATEINDEX_INELEMENTNAME,
		SGML_STC_LETTER_TYPE_SPECIFICWS,
		0,
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		SGML_PARSER_STATE_INELEMENTNAME_ACTUAL,
		0,
		SGML_PARSER_STATE_INATTRIBUTENAME,
		SGML_PARSER_STATE_INELEMENTNAME | SGML_PARSER_STATE_INELEMENTNAME_ACTUAL | SGML_PARSER_STATE_INELEMENTCLOSURE | SGML_PARSER_STATE_INCOMMENTGOTEXCLAMATION | SGML_PARSER_STATE_INCOMMENTGOTDASH1
	},
	{
		SGML_PARSER_STATEINDEX_INELEMENTNAME,
		SGML_STC_LETTER_TYPE_SPECIFICWS,
		0,
		0,
		0,
		0,
		SGML_PARSER_STATE_INELEMENTNAME_ACTUAL,
		0
	},
	{	// Once we get our first non-space character we safely start getting the name itself, as long as the state isn't ACTUAL.
		SGML_PARSER_STATEINDEX_INELEMENTNAME,
		SGML_STC_LETTER_TYPE_NOTWS,
		0,
		SGML_STC_FLAG_UPDATE_STATE | SGML_STC_FLAG_INCL_IN_BUFFER,
		0,
		0,
		SGML_PARSER_STATE_INELEMENTNAME_ACTUAL,
		SGML_PARSER_STATE_INELEMENTNAME_ACTUAL,
		0
	},
	/*
	 * SGML_PARSER_STATEINDEX_INATTRIBUTENAME
	 * AttributeName mode rules
	 *
	 * State changers:
	 *		/                               (update set state in closure element)
	 *		>                               (divert text)
	 *		=                               (divert attrval)
	 *		first nonspace                  (update set state in actual attrname)
	 *		first space after nonspace set  (divert attrname)
	 */	
	{
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'/',
		SGML_STC_FLAG_UPDATE_STATE | SGML_STC_FLAG_INCL_IN_BUFFER,
		0,
		0,
		SGML_PARSER_STATE_INELEMENTCLOSURE,
		SGML_PARSER_STATE_INELEMENTCLOSURE,
		0		
	},
	{
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'>',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INTEXT,
		0,
		0,
		SGML_PARSER_STATE_INTEXT,
		SGML_PARSER_STATE_INATTRIBUTENAME | SGML_PARSER_STATE_INATTRIBUTENAME_ACTUAL | SGML_PARSER_STATE_INELEMENT | SGML_PARSER_STATE_INELEMENTCLOSURE
	},
	{
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'=',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_NS,
		0,
		0,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_NS,
		SGML_PARSER_STATE_INATTRIBUTENAME | SGML_PARSER_STATE_INATTRIBUTENAME_ACTUAL
	},
	{	// Ignore unwanted spaces.
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		SGML_STC_LETTER_TYPE_SPECIFICWS,
		0,
		0,
		0,
		0,
		SGML_PARSER_STATE_INATTRIBUTENAME_ACTUAL,
		0,
		0
	},
	{	// Referential
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		SGML_STC_LETTER_TYPE_SPECIFICWS,
		0,
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		SGML_PARSER_STATE_INATTRIBUTENAME_ACTUAL,
		0,
		SGML_PARSER_STATE_INATTRIBUTENAME,
		SGML_PARSER_STATE_INATTRIBUTENAME_ACTUAL
	},	
	{
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		SGML_STC_LETTER_TYPE_NOTWS,
		0,
		SGML_STC_FLAG_UPDATE_STATE | SGML_STC_FLAG_INCL_IN_BUFFER,
		0,
		0,
		SGML_PARSER_STATE_INATTRIBUTENAME_ACTUAL,
		SGML_PARSER_STATE_INATTRIBUTENAME_ACTUAL,
		0
	},	
	/*
	 * SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_NS
	 * AttributeValue_NoSpaces mode rules
	 *
	 * Specific changes:
	 *    >	  (divert SGML_PARSER_STATEINDEX_INTEXT)
	 *		' '  (divert SGML_PARSER_STATEINDEX_INATTRIBUTENAME)
	 *		"    (divert SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_S)
	 *		'    (divert SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_S)
	 */	
	{
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_NS,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'>',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INTEXT,
		0,
		0,
		SGML_PARSER_STATE_INTEXT,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_NS | SGML_PARSER_STATE_INELEMENT
	},
	{
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_NS,
		SGML_STC_LETTER_TYPE_SPECIFICWS,
		0,
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		0,
		0,
		SGML_PARSER_STATE_INATTRIBUTENAME,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_NS
	},
	{
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_NS,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'"',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_S,
		0,
		0,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_S | SGML_PARSER_STATE_INATTRIBUTEVALUE_DBLQUOTE,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_NS
	},
	{
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_NS,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'\'',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_S,
		0,
		0,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_S,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_NS
	},
	/*
	 * SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_S
	 * AttributeValue_NoSpaces mode rules
	 *
	 * Specific changes:
	 *		"  (divert attrname)
	 *		'  (divert attrname)
	 *
	 */
	{
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_S,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'"',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_DBLQUOTE,
		0,
		SGML_PARSER_STATE_INATTRIBUTENAME,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_S | SGML_PARSER_STATE_INATTRIBUTEVALUE_DBLQUOTE	
	},
	{
		SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_S,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'\'',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INATTRIBUTENAME,
		0,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_DBLQUOTE,
		SGML_PARSER_STATE_INATTRIBUTENAME,
		SGML_PARSER_STATE_INATTRIBUTEVALUE_S	
	},
	/*
	 * SGML_PARSER_STATEINDEX_INCOMMENT
	 * Comment mode rules
	 *
	 * Specific changes:
	 *    >       if dash1 and dash2, divert text
	 *    not -   unset SGML_PARSER_STATE_INCOMMENTGOTDASH1
	 *    -       set SGML_PARSER_STATE_INCOMMENTGOTDASH1
	 *    -       if dash1, set dash2
	 */	
	{
		SGML_PARSER_STATEINDEX_INCOMMENT,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'>',
		SGML_STC_FLAG_DIVERT | SGML_STC_FLAG_UPDATE_STATE,
		SGML_PARSER_STATEINDEX_INTEXT,
		SGML_PARSER_STATE_INCOMMENTGOTDASH1 | SGML_PARSER_STATE_INCOMMENTGOTDASH2,
		0,
		SGML_PARSER_STATE_INTEXT,
		SGML_PARSER_STATE_INCOMMENT | SGML_PARSER_STATE_INCOMMENTGOTDASH1 | SGML_PARSER_STATE_INCOMMENTGOTDASH2
	},
	{
		SGML_PARSER_STATEINDEX_INCOMMENT,
		SGML_STC_LETTER_TYPE_NOT,
		'-',
		SGML_STC_FLAG_UPDATE_STATE | SGML_STC_FLAG_INCL_IN_BUFFER,
		0,
		0,
		0,
		0,
		SGML_PARSER_STATE_INCOMMENTGOTDASH1 | SGML_PARSER_STATE_INCOMMENTGOTDASH2
	},
	{
		SGML_PARSER_STATEINDEX_INCOMMENT,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'-',
		SGML_STC_FLAG_UPDATE_STATE,
		0,
		0,
		SGML_PARSER_STATE_INCOMMENTGOTDASH1,
		SGML_PARSER_STATE_INCOMMENTGOTDASH1,
		0
	},
	{
		SGML_PARSER_STATEINDEX_INCOMMENT,
		SGML_STC_LETTER_TYPE_SPECIFIC,
		'-',
		SGML_STC_FLAG_UPDATE_STATE,
		0,
		0,
		SGML_PARSER_STATE_INCOMMENTGOTDASH2,
		SGML_PARSER_STATE_INCOMMENTGOTDASH2,
		0
	},

};

SGML_PARSER *sgmlParserNew()
{
	SGML_PARSER *ret = (SGML_PARSER *)malloc(sizeof(SGML_PARSER));

	if (!ret)
		return NULL;

	memset(ret, 0, sizeof(SGML_PARSER));

	ret->type = SGML_EXTENSION_TYPE_CUSTOM;

	return ret;
}

unsigned long sgmlParserInitialize(SGML_PARSER *parser, enum SgmlExtensionType type, SGML_HANDLERS *handlers, void *userContext)
{
	memset(parser, 0, sizeof(SGML_PARSER));

	parser->type                 = type;
	parser->internal.userContext = userContext;
	parser->internal.state       = SGML_PARSER_STATE_INTEXT;
			
	_sgmlParserInitializeStateTable(parser);

	if (handlers)
		memcpy(&parser->handlers, handlers, sizeof(SGML_HANDLERS));

	switch (parser->type)
	{
		case SGML_EXTENSION_TYPE_HTML:
			parser->internal.extensionContext = (void *)sgmlExtensionHtmlNew(parser);
			break;
		case SGML_EXTENSION_TYPE_XML:
			parser->internal.extensionContext = (void *)sgmlExtensionXmlNew(parser);
			break;
		default:
			break;
	}	

	_sgmlParserInitializeStateTableRules(parser);

	return 1;
}

void _sgmlParserInitializeStateTable(SGML_PARSER *parser)
{
	parser->stateTable                 = sgmlStateTable;
	parser->stateTableElements         = sizeof(sgmlStateTable) / sizeof(SGML_STATE_TABLE);
	parser->stateTableRules            = sgmlStateTableRules;
	parser->stateTableRuleElements     = sizeof(sgmlStateTableRules) / sizeof(SGML_STATE_TABLE_RULE);

	parser->internal.onStateChange     = _sgmlOnStateChange;
	parser->internal.onDivert          = _sgmlOnDivert;

	parser->internal.extensionContext  = NULL;

	parser->internal.currentState      = &sgmlStateTable[0];
	parser->internal.currentBuffer     = NULL;
	parser->internal.currentBufferSize = 0;
}

void _sgmlParserInitializeStateTableRules(SGML_PARSER *parser)
{
	unsigned long lastTableIndex = -1;
	unsigned long x = 0, lastChange = 0;

	for (lastTableIndex = SGML_PARSER_STATEINDEX_INTEXT,
			x = 0;
			x <= parser->stateTableRuleElements;
			x++)
	{
		/*
		 * If the index changed.
		 */
		if ((x == parser->stateTableRuleElements) || (parser->stateTableRules[x].stateIndexId != lastTableIndex))
		{
			parser->stateTable[lastTableIndex].ruleSize = x - lastChange; 
			parser->stateTable[lastTableIndex].rules    = &(parser->stateTableRules[0]) + lastChange;

			lastTableIndex = parser->stateTableRules[x].stateIndexId;
			lastChange = x;
		}
	}
}

void sgmlParserDestroy(SGML_PARSER *parser, unsigned char destroyParser)
{
	switch (parser->type)
	{
		case SGML_EXTENSION_TYPE_HTML:
			sgmlExtensionHtmlDestroy(parser, (SGML_EXTENSION_HTML *)parser->internal.extensionContext);
			break;
		case SGML_EXTENSION_TYPE_XML:
			sgmlExtensionXmlDestroy(parser, (SGML_EXTENSION_XML *)parser->internal.extensionContext);
			break;
		default:
			break;
	}

	if (destroyParser)
		free(parser);
}

unsigned long sgmlParserParseString(SGML_PARSER *parser, const char *string, const unsigned long stringLength)
{
	unsigned long ret;

	if (parser->handlers.preparse)
		parser->handlers.preparse(parser, parser->internal.userContext);

	ret = _sgmlParseChunk(parser, string, stringLength);
	
	if (parser->handlers.postparse)
		parser->handlers.postparse(parser, parser->internal.userContext);

	if (parser->internal.lastAttributeName)
		free(parser->internal.lastAttributeName);
	if (parser->internal.lastElementName)
		free(parser->internal.lastElementName);
	if (parser->internal.currentBuffer)
		free(parser->internal.currentBuffer);

	return ret;
}

unsigned long sgmlParserParseFile(SGML_PARSER *parser, const char *file)
{
	unsigned long chunkSize, ret = 1;
	char chunk[4096];
	FILE *fd;

	if ((fd = fopen(file, "r")) == NULL)
		return 0;

	if (parser->handlers.preparse)
		parser->handlers.preparse(parser, parser->internal.userContext);

	while ((ret) && ((chunkSize = fread(chunk, 1, sizeof(chunk), fd)) > 0))
		ret = _sgmlParseChunk(parser, chunk, chunkSize);

	if (parser->handlers.postparse)
		parser->handlers.postparse(parser, parser->internal.userContext);

	fclose(fd);

	if (parser->internal.lastAttributeName)
		free(parser->internal.lastAttributeName);
	if (parser->internal.lastElementName)
		free(parser->internal.lastElementName);
	if (parser->internal.currentBuffer)
		free(parser->internal.currentBuffer);

	return ret;
}

void sgmlParserExtensionSetParam(SGML_PARSER *parser, unsigned long param, void *value)
{
	if (parser->internal.setExtensionParam)
		parser->internal.setExtensionParam(parser, parser->internal.extensionContext, param, value);
}

void sgmlParserExtensionGetParam(SGML_PARSER *parser, unsigned long param, void *value)
{
	if (parser->internal.getExtensionParam)
		parser->internal.getExtensionParam(parser, parser->internal.extensionContext, param, value);
}

unsigned long _sgmlParseChunk(SGML_PARSER *parser, const char *chunk, const unsigned long chunkSize)
{
	unsigned long ret = 1, includeInBuffer = 0, divert = 0, oldState = 0, newIndex = 0, startPosition = 0, addPosition;
	register int x = 0, ruleEnum = 0;

	for (; x < chunkSize; x++)
	{
		includeInBuffer = 1;

		for (ruleEnum = 0;
				ruleEnum < parser->internal.currentState->ruleSize;
				ruleEnum++)
		{
			unsigned char matchLetter = 0;

			switch (parser->internal.currentState->rules[ruleEnum].letterType)
			{
				case SGML_STC_LETTER_TYPE_SPECIFIC:
					if (parser->internal.currentState->rules[ruleEnum].letter == chunk[x])
						matchLetter = 1;
					break;
				case SGML_STC_LETTER_TYPE_NOT:
					if (parser->internal.currentState->rules[ruleEnum].letter != chunk[x])
						matchLetter = 1;
					break;
				case SGML_STC_LETTER_TYPE_SPECIFICWS:
					switch (chunk[x])
					{
						case ' ':
						case '\t':
						case '\r':
						case '\n':
							matchLetter = 1;
							break;
					}
					break;
				case SGML_STC_LETTER_TYPE_NOTWS:
					switch (chunk[x])
					{
						case ' ':
						case '\t':
						case '\r':
						case '\n':
							matchLetter = 0;
							break;
						default:
							matchLetter = 1;
							break;
					}
					break;
				case SGML_STC_LETTER_TYPE_ANY:
					matchLetter = 1;
					break;
				default:
					break;
			}

			if ((matchLetter)
				 && ((!parser->internal.currentState->rules[ruleEnum].isState) || (parser->internal.state & parser->internal.currentState->rules[ruleEnum].isState))
				 && ((!parser->internal.currentState->rules[ruleEnum].notState) || (!(parser->internal.state & parser->internal.currentState->rules[ruleEnum].notState))))
			{
				/*
				 * Update state?
				 */

				if (parser->internal.currentState->rules[ruleEnum].flags & SGML_STC_FLAG_UPDATE_STATE)
				{
					unsigned long newState = (parser->internal.state | parser->internal.currentState->rules[ruleEnum].addState);

					if (parser->internal.currentState->rules[ruleEnum].remState)
						newState &= ~(parser->internal.currentState->rules[ruleEnum].remState);

					oldState = parser->internal.state;
					parser->internal.state = newState;

					if (parser->internal.onStateChange)
						parser->internal.onStateChange(parser, oldState, newState);
				}

				/*
				 * Include in buffer?
				 */
				if (!(parser->internal.currentState->rules[ruleEnum].flags & SGML_STC_FLAG_INCL_IN_BUFFER))
					includeInBuffer = 0;

				/*
				 * Divert?
				 */
				if (parser->internal.currentState->rules[ruleEnum].flags & SGML_STC_FLAG_DIVERT)
				{
					parser->internal.currentState = &parser->stateTable[(newIndex = parser->internal.currentState->rules[ruleEnum].divertTableId)];

					divert = 1;
				}

				break;
			}
		}
		
		if (divert)
		{
			if (!includeInBuffer)
			{
				_sgmlParserAppendBuffer(parser, chunk, startPosition, x);
				addPosition  = x - startPosition;
			}
			else
			{
				_sgmlParserAppendBuffer(parser, chunk, startPosition, x + 1);
				addPosition = (x+1) - startPosition;
			}

			if (parser->internal.currentBuffer)
				parser->internal.currentBuffer[parser->internal.currentBufferSize] = 0;

			if (parser->internal.onDivert)
				parser->internal.onDivert(parser, newIndex, oldState, parser->internal.state, parser->internal.currentBuffer, parser->internal.currentBufferSize);

			_sgmlParserResetBuffer(parser);
		
			startPosition += addPosition + 1;

			divert = 0;
		}
		else if (!includeInBuffer) 
		{
			_sgmlParserAppendBuffer(parser, chunk, startPosition, x);
			addPosition = x - startPosition;

			startPosition += addPosition + 1;
		}
	}
	
	if (x != 0)
		_sgmlParserAppendBuffer(parser, chunk, startPosition, x-1);

	return ret;
}

void _sgmlParserAppendBuffer(SGML_PARSER *parser, const char *chunk, unsigned long startOffset, unsigned long endOffset)
{
	unsigned long copyLength = endOffset - startOffset;
	char *saveBuf = NULL;

	if (endOffset <= startOffset)
		return;
	
	if (!parser->internal.currentBuffer)
		parser->internal.currentBuffer = (char *)malloc(copyLength + 1);
	else
		parser->internal.currentBuffer = (char *)realloc((saveBuf = parser->internal.currentBuffer), parser->internal.currentBufferSize + copyLength + 1);

	if (!parser->internal.currentBuffer)
	{
		if (saveBuf)
			free(saveBuf);
	}	
	else
		memcpy(parser->internal.currentBuffer + parser->internal.currentBufferSize, chunk + startOffset, copyLength);

	parser->internal.currentBufferSize += copyLength;
}

void _sgmlParserResetBuffer(SGML_PARSER *parser)
{
	if (parser->internal.currentBuffer)
		free(parser->internal.currentBuffer);

	parser->internal.currentBuffer     = NULL;
	parser->internal.currentBufferSize = 0;
}

void _sgmlOnStateChange(SGML_PARSER *parser, unsigned long oldState, unsigned long newState)
{
}

void _sgmlOnDivert(SGML_PARSER *parser, unsigned long newIndex, unsigned long oldState, unsigned long newState, const char *lastBuffer, unsigned long lastBufferSize)
{
	char *buffer = (lastBuffer)?(char *)lastBuffer:"";

	switch (newIndex)
	{
		case SGML_PARSER_STATEINDEX_INELEMENTNAME:
			if (oldState & SGML_PARSER_STATE_INTEXT)
			{
				if (parser->handlers.textNew)
					parser->handlers.textNew(parser, parser->internal.userContext, buffer);
			}
			break;
		case SGML_PARSER_STATEINDEX_INATTRIBUTENAME:
			if (oldState & SGML_PARSER_STATE_INELEMENTNAME)
			{
				if (parser->handlers.elementBegin)
					parser->handlers.elementBegin(parser, parser->internal.userContext, buffer);
						
				if (parser->internal.lastElementName)
					free(parser->internal.lastElementName);
				
				parser->internal.lastElementName = (char *)strdup(buffer);
			}
			if (oldState & SGML_PARSER_STATE_INATTRIBUTENAME)
			{
				if (parser->handlers.attributeNew)
					parser->handlers.attributeNew(parser, parser->internal.userContext, buffer, "");
			}
			if ((oldState & SGML_PARSER_STATE_INATTRIBUTEVALUE_NS) 
					|| (oldState & SGML_PARSER_STATE_INATTRIBUTEVALUE_S))
			{
				if (parser->handlers.attributeNew)
					parser->handlers.attributeNew(parser, parser->internal.userContext, parser->internal.lastAttributeName, buffer);
			}
			break;
		case SGML_PARSER_STATEINDEX_INATTRIBUTEVALUE_NS:
			if (parser->handlers.attributeNew)
			{
				if (parser->internal.lastAttributeName)
					free(parser->internal.lastAttributeName);

				parser->internal.lastAttributeName = (char *)strdup(buffer);
			}
			break;
		case SGML_PARSER_STATEINDEX_INTEXT:
			if (oldState & SGML_PARSER_STATE_INELEMENTCLOSURE)
			{
				char *slashPos = 0;
			
				if (!(oldState & SGML_PARSER_STATE_INELEMENTNAME))	
					buffer = parser->internal.lastElementName;

				if ((buffer) && ((slashPos = (char *)strrchr(buffer, '/'))) && (slashPos != buffer))
				{
					*slashPos = 0;

					if (parser->handlers.elementBegin)
					{
						parser->handlers.elementBegin(parser, parser->internal.userContext, buffer);
					
						if (parser->internal.lastElementName)
							free(parser->internal.lastElementName);

						parser->internal.lastElementName = (char *)strdup(buffer);
					}
				}
				else if (oldState & SGML_PARSER_STATE_INELEMENTNAME)
					buffer++;
					
				if (parser->handlers.elementEnd)
					parser->handlers.elementEnd(parser, parser->internal.userContext, buffer);
			}
			else if (oldState & SGML_PARSER_STATE_INELEMENTNAME)
			{	
				if (parser->handlers.elementBegin)
					parser->handlers.elementBegin(parser, parser->internal.userContext, buffer);
						
				if (parser->internal.lastElementName)
					free(parser->internal.lastElementName);

				parser->internal.lastElementName = (char *)strdup(buffer);
			}
			else if (oldState & SGML_PARSER_STATE_INATTRIBUTENAME && buffer && *buffer)
			{
				if (parser->handlers.attributeNew)
					parser->handlers.attributeNew(parser, parser->internal.userContext, buffer, "");
			}
			else if (oldState & SGML_PARSER_STATE_INATTRIBUTEVALUE_NS)
			{
				if (parser->handlers.attributeNew)
					parser->handlers.attributeNew(parser, parser->internal.userContext, parser->internal.lastAttributeName, lastBuffer);
			}
			else if (oldState & SGML_PARSER_STATE_INCOMMENT)
			{
				if (parser->handlers.commentNew)
					parser->handlers.commentNew(parser, parser->internal.userContext, buffer);
			}
			break;
	}
}

