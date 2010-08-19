/*
 * 	mail.cpp
 *	
 *	Implementation of the Switch Mail component
 *
 *	(C)2005-06 Phaistos Networks, S.A.
 *
 *	http://www.hcs.harvard.edu/~thurston/ua/pm-tips.html
 *
 *	Make sure you use the w flag (wait for the program in the action line to finish
 *	beore continuing, additionally catching the return code. Otherwise, Procmail will spawn off
 *	the program and leave it executing on its own.
 *	Mote: do not use the f flag
 * 
 * 	:0w
 * 	ENV_TO ?? .
 * 	| /home/system/Tools/Pathfinder/Mail/mailproc $ENV_TO
 * 	EXITCODE=$?
 *	DEFAULT=/dev/null
 *
 *	check the flags (check link below ) for what those variables are needed for
 *
 *	So that procmail will catch the exit error of the filter. Check /usr/include/sysexits.h for what
 *	to return on exit from mailproc to procmail
 *
 *	procmail flags are here: http://laku19.adsl.netsonic.fi/era/procmail/quickref.html
 */

#if defined(SWITCH_ENABLE_MYSQL)
#include "mail.h"
#include "data.h"
#include "switch.h"
#include "text.h"
#include "date.h"
#include "crawler.h"

#define LINELEN  76

#define SWITCHMAIL_DEFAULT_CONTENTTYPE 		"text/plain; charset=us-ascii"
#define SWITCHMAIL_DEFAULT_CTYPEFORUNKNOWNCTE 	"application/octet-stream"

const char *SwitchMail::GetTransferEncodingName(contenttransferencoding_t t)
{
	switch (t)
	{
		case CTE_7BIT:
			return "7bit";

		case CTE_8BIT:
			return "8bit";

		case CTE_BASE64:
			return "base64";

		case CTE_BINARY:
			return "binary";

		case CTE_QUOTED_PRINTABLE:
			return "quoted-printable";

		default:
			return " - unknown - ";
	}

	assert(!"Unknown transfer encoding type");
	return NULL;

}


SwitchMail::contenttransferencoding_t SwitchMail::GetTransferEncodingType(const char *name)
{
	if (!Strcasecmp(name, "7bit"))
		return CTE_7BIT;
	else if (!Strcasecmp(name, "8bit"))
		return CTE_8BIT;
	else if (!Strcasecmp(name, "quoted-printable"))
		return CTE_QUOTED_PRINTABLE;
	else if (!Strcasecmp(name, "base64"))
		return CTE_BASE64;
	else if (!Strcasecmp(name, "binary"))
		return CTE_BINARY;
	else
		return CTE_UNKOWN;
}



SwitchMail::contenttransferencoding_t SwitchMail::GuessContentTransferEncoding(CBuffer *b, const char *cType)
{
	if (!b)
		return GuessContentTransferEncoding(NULL, 0, cType);
	else
		return GuessContentTransferEncoding((uint8_t *)b->GetData(), b->GetLength(), cType);
}

SwitchMail::contenttransferencoding_t SwitchMail::GuessContentTransferEncoding(uint8_t *data, uint dataLen, const char *cType)
{       

	if (cType)
	{
		if (!Strncasecmp(cType, "MULTIPART/", 10) || !Strncasecmp(cType, "MESSAGE/", 8))
		{
			/* Mutltpart or MESSAGE go as is.
			* 
				If an entity is
				   of type "multipart" the Content-Transfer-Encoding is not permitted to
				   have any value other than "7bit", "8bit" or "binary". 
			*/

			return CTE_8BIT;
		}
		else if (Strncasecmp(cType, "TEXT/", 5))
		{
			/* If not text/ -> we consider it binary and go for base64 directly */

			return CTE_BASE64;
		}
	}

        uint maxLen = 0, charsInThisLine = 0,  len = dataLen;
        uint eightBitChars 	= 0;
        
        while (dataLen--)
        {

		if (*data < 9)
		{
			/* Impossible to represent those in ASCII */


			return CTE_BASE64;
		}
		else if (*data == '\n')
		{
			if (charsInThisLine > maxLen)
				maxLen = charsInThisLine;
			charsInThisLine = 0;
		}
		else if (*data >= 128)
		{
			
			eightBitChars++;
			charsInThisLine++;
		}
		else 
		{
			if (charsInThisLine == 0 && *data == '.')
			{
				/* Whenever we find a . as begining of a line, we switch to base64.
				 * Even if it doesnt signify end of line, stupid sendmail stips it!
				 */
			
				return CTE_BASE64;
			}
	
			charsInThisLine++;
		}

		data++;

	}

	if (charsInThisLine > maxLen)
		maxLen = charsInThisLine;



        if (!eightBitChars && maxLen<=LINELEN)                                                                         
                return CTE_7BIT;

	if (eightBitChars && ( (int)((float)eightBitChars * 100.0 / (float)len) > 25 ))
	{
		/* http://support.microsoft.com/default.aspx?scid=kb;EN-US;q278134 */

		return CTE_BASE64;
	}

        if (maxLen > LINELEN)                                                                                
	{
		/* We must encode  */

		return CTE_QUOTED_PRINTABLE;
	}

	return CTE_8BIT;
}                                                                                                                 


void SwitchMail::EncodeBase64(CBuffer *input, CBuffer *outputBuf, bool forHeaders)
{
	EncodeBase64((uint8_t *)input->GetData(), input->GetLength(), outputBuf, forHeaders);
}

void SwitchMail::EncodeBase64(uint8_t *data, size_t length, CBuffer *outputBuf, bool forHeaders)
{
	unsigned int i;
	uint8_t dtable[256];


	for (i = 0; i < 9; i++)
    	{
	      dtable[i] 	= 'A' + i;
	      dtable[i + 9] 	= 'J' + i;
	      dtable[26 + i] 	= 'a' + i;
	      dtable[26 + i + 9]= 'j' + i;
    	}

  	for (i = 0; i < 8; i++)
    	{
	      dtable[i + 18] 		= 'S' + i;
	      dtable[26 + i + 18] 	= 's' + i;
    	}

  	for (i = 0; i < 10; i++)
      		dtable[52 + i] = '0' + i;
  	dtable[62] = '+';
  	dtable[63] = '/';


	/* Make sure we have enough space to hold the base64 encoded data */

	uint mm = length / 3 * 4, required = 32 + mm + (mm / LINELEN * 2);
	if (outputBuf->GetSize() - outputBuf->GetLength() <= required)
		outputBuf->Expand(required - (outputBuf->GetSize() - outputBuf->GetLength()));

	uint8_t *output 	= (uint8_t *)(outputBuf->GetData() + outputBuf->GetLength()), *initial = output;
	int nChars = 0, lineLength = 0;
	uint8_t ig[3] = {0 , 0 , 0};


	while (length--)
	{
	
		if (nChars < 3)
			ig[nChars++] = *data++;
		else
		{
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[0] >> 2];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[0] & 3) << 4) | (ig[1] >> 4)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[1] & 0xf) << 2) | (ig[2] >> 6)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[2] & 0x3f];

			nChars = 1;
			ig[0] = *data++;
			ig[1] = 0;
			ig[2] = 0;
		}
	}

	/* There bound to be some characters left to process */

	switch (nChars)
	{
		case 3:
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[0] >> 2];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[0] & 3) << 4) | (ig[1] >> 4)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[1] & 0xf) << 2) | (ig[2] >> 6)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[2] & 0x3f];
			break;

		case 2:
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[0] >> 2];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[0] & 3) << 4) | (ig[1] >> 4)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[1] & 0xf) << 2) | (ig[2] >> 6)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = '=';
			break;

		case 1:
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[ig[0] >> 2];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = dtable[((ig[0] & 3) << 4) | (ig[1] >> 4)];
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = '=';
			if (lineLength++ == LINELEN) { *output++ = '\r'; *output++ = '\n'; if (forHeaders) *output++ = '\t'; lineLength = 1; }
			*output++ = '=';
			break;

	}


	if (required <= (uint)(output - initial))
	{
		fprintf(stderr, "REQUIRED M O R E data than preallocated!\n");
	}



	outputBuf->SetLength(outputBuf->GetLength() + (output - initial));
}

void SwitchMail::EncodeQuotedPrintable(CBuffer *input, CBuffer *outputBuf)
{
	SwitchMail::EncodeQuotedPrintable((uint8_t *)input->GetData(), input->GetLength(), outputBuf);
}

void SwitchMail::EncodeQuotedPrintable(uint8_t *input, size_t length, CBuffer *outputBuf)
{
	/* Make sure we have enough space to hold the base64 encoded data */

	uint required = (length * 3) + 126;
	if (outputBuf->GetSize() - outputBuf->GetLength() <= required)
		outputBuf->Expand(required - (outputBuf->GetSize() - outputBuf->GetLength()));

	uint8_t *data 	= input, c;
	uint8_t *output 	= (uint8_t *)(outputBuf->GetData() + outputBuf->GetLength()), *initial = output;
	uint charsInThisLine 	= 0, a, b;
	bool useHex;

	while (length--)
	{
		c 	= *data++;
		useHex 	= false;

		switch (c)
		{

			case 33 ... 60:
			case 62 ... 126:
				if (c == '.' && charsInThisLine == 0)
				{
					/* Stupid sendmail strips it away!*/
					useHex = true;
				}
				else
				{
					*output++ = c;
					charsInThisLine++;
				}
				break;

			case '\r':
				/* Ignore \r -- we handle \n as CRLF */
				break;

			case '\t':
			case ' ':
				if (*data != '\n' && *data != '\r' && *data)
				{
					*output++ = c;
					charsInThisLine++;
				}
				else
					useHex = true;
				break;

			case '\n':
				*output++ = '\r';
				*output++ = '\n';
				charsInThisLine = 0;
				break;

			default:
				useHex = true;
		}

		if (*data && (charsInThisLine + (useHex ? 3 : 0) > LINELEN))
		{
			*output++ = '=';
			*output++ = '\n';
			charsInThisLine = 0;
		}

		if (useHex)
		{
			*output++ = '=';
			a = c / 16;
			b = c % 16;
			*output++  = a < 10 ? '0' + a : 'A' + a - 10;
			*output++  = b < 10 ? '0' + b : 'A' + b - 10;
			charsInThisLine+=3;
		}


	}


	if (required <= (uint)(output - initial))
	{
		fprintf(stderr, "REQUIRED M O R E data than preallocated!\n");
	}

	outputBuf->SetLength(outputBuf->GetLength() + (output - initial));


}


bool SwitchMail::ShouldEncodeHeaderValue(uint8_t *data)
{
	while (*data)
	{
		if (*data < 32 || *data > 128)
			return true;
		data++;
	}

	return false;
}


unsigned int SwitchMail::DispatchMessage(CDatabase *database, CIOBuffer &buffer, const char *from, const char *to, const char *subject, CBuffer *body, const char *cType, const char *service, CXmlrpcStruct *opts, uint8_t priority)
{
	DEFINE_STACKFRAME_SENTINEL;

	CSwitchVector<email_t *, DeleteDestructor> emailsFrom, emailsTo;
	uint totalInstructions = 0, len;
	CBuffer cmd, queryBuf;
	IXmlrpcValue *iv;
	// Disabled pending bug fixes
	const bool validateEmails = false; opts && (iv = opts->Find("validate_emails", 15)) && iv->GetType() == IXmlrpcValue::XMLRPC_BOOLEAN && (bool)*(CXmlrpcBoolean *)iv == true;
	struct email_validator_ctx *vc = NULL;

	if (!from || !to || !service)
	{
		/* Sanity check */

		SwitchDebug("(from|to|service) not defined\n");

		return 0;
	}	


	char *escFrom = CDatabase::Escape(from), *escTo = CDatabase::Escape(to), *escService = CDatabase::Escape(service);
	queryBuf.AppendFmt("INSERT INTO v10_aux.outgoing_mail_queue SET id = 0, ts = UNIX_TIMESTAMP(), to_info = '%s', from_info = '%s', service = '%s', pri = %u, data = '", escTo, escFrom,escService, priority);
	Free(escFrom);
	Free(escTo);
	Free(escService);


	VALIDATE_STACKFRAME;


	buffer.Flush();
	buffer.EnsureSize(body->GetLength() << 1);
	buffer.PushBinary(&totalInstructions, sizeof(totalInstructions));


	if (validateEmails)
		vc = new email_validator_ctx();

	/* From */

	cmd.Flush();
	ExtractEmails(from, 0, &emailsFrom);

	if (vc)
		ValidateEmailsV(&emailsFrom, vc);

	if (emailsFrom.GetSize() == 0)
	{
		/* No 'FROM 'specified */

		SwitchDebug("FROM not specified\n");
		delete vc;
		return 0;
	}
	cmd.AppendFmt("MAIL FROM:<%s>\r\n", emailsFrom[0]->email);

	len = cmd.GetLength();
	buffer.PushBinary(&len, sizeof(len));
	buffer.PushBinary(cmd.GetData(), len);
	totalInstructions++;

	VALIDATE_STACKFRAME;

	/* To. Please note: All emails should be specified here, To, Cc, Bcc  as plain emails */

	ExtractEmails(to, 0, &emailsTo);

	if (vc)
		ValidateEmailsV(&emailsTo, vc);

	if (emailsTo.GetSize() == 0)
	{
		/* No 'TO' specified */

		SwitchDebug("TO not specified[%s]\n", to);

		VALIDATE_STACKFRAME;



		delete vc;
		return 0;
	}

	
	/* Get 'cc' and 'bcc', if possible */
	
	CSwitchVector<email_t *, DeleteDestructor> extraDestinations;
	if (opts && (iv = opts->Find("headers", 7)) && iv->GetType() == IXmlrpcValue::XMLRPC_STRUCT)
	{
		CBuffer *hVal;
		CXmlrpcStruct::value_list_t *vl;
		CXmlrpcStruct *headers = (CXmlrpcStruct *)iv;

		for (vl = headers->members; vl; vl = vl->next)
		{
			if (!Strcasecmp(vl->name, "CC") || !Strcasecmp(vl->name, "BCC"))
			{
				if (vl->value->GetType() == IXmlrpcValue::XMLRPC_STRING)
				{
					hVal = (CBuffer *)(CXmlrpcString *)vl->value;
					ExtractEmails(hVal->GetData(), EXTRACT_EMAILS_NO_NAME, &extraDestinations);

				}

			}
		}

		if (vc)
			ValidateEmailsV(&extraDestinations, vc);
	}

	VALIDATE_STACKFRAME;

	if (emailsTo.GetSize() + extraDestinations.GetSize() > 10 && Strstr(service, "office"))
	{
		/* We do NOT allow more than 10 emails to be sent at the same time from office */

		SwitchDebug("Attempted to send mail to %d recipients from service '%s'\n", emailsTo.GetSize() + extraDestinations.GetSize(), service);

		delete vc;
		return 0;
	}


	for (uint i = 0; i < emailsTo.GetSize(); i++)
	{

		cmd.Flush();
		cmd.AppendFmt("RCPT TO:<%s>\r\n", emailsTo[i]->email);


		len = cmd.GetLength();
		buffer.PushBinary(&len, sizeof(len));
		buffer.PushBinary(cmd.GetData(), len);
		totalInstructions++;
	}

	for (uint i = 0; i < extraDestinations.GetSize(); i++)
	{

		cmd.Flush();
		cmd.AppendFmt("RCPT TO:<%s>\r\n", extraDestinations[i]->email);

		len = cmd.GetLength();
		buffer.PushBinary(&len, sizeof(len));
		buffer.PushBinary(cmd.GetData(), len);
		totalInstructions++;
	}



	/* Prepare data */

	cmd.Flush();
	cmd.Append("DATA\r\n", 6);

	len = cmd.GetLength();
	buffer.PushBinary(&len, sizeof(len));
	buffer.PushBinary(cmd.GetData(), len);



	totalInstructions++;




	uint curLen = buffer.GetLength(); 		/* Position/offset within buffer for data len */
	len = 0;  					/* Initial data length */
	buffer.PushBinary(&curLen, sizeof(curLen));


	/* Construct our body data */


	VALIDATE_STACKFRAME;
	
	BuildMessageBody(buffer, &emailsFrom, &emailsTo, subject, body, cType, opts, vc);

	VALIDATE_STACKFRAME;




	/* End of DATA (marked by a single . in a line )
	 * CR><LF>.<CR><LF>
	 */

	buffer.Append("\r\n.\r\n", 5);


	/* Update body data length (including '.'\r\n terminator ) */

	len = buffer.GetLength() - curLen - sizeof(uint);
	*(uint *)(buffer.GetData() + curLen) = len;


	/* Update total instructions */

	*((uint *)(buffer.GetData())) = totalInstructions;


	VALIDATE_STACKFRAME;




	queryBuf.EnsureCapacity((buffer.GetLength() << 1) + 1024);
	queryBuf.AdvanceLength(CDatabase::EscapeTo(buffer.GetData(), queryBuf.GetData() + queryBuf.GetLength(), buffer.GetLength()));
	queryBuf.Append('\'');

	if (unlikely(!database->SendQuery(false, queryBuf)))
	{
		delete vc;
		return -1;
	}

	VALIDATE_STACKFRAME;


	/* buffer.GetData() + curLen -> (uint)len of data, data. Exclude 3 trailing bytes ! (used for .\r\n) */

	delete vc;

	VALIDATE_STACKFRAME;
	return curLen;
}



static void AppendTextCarefully(char *p, const size_t len, CIOBuffer *out)
{
	 /*
	  * 	http://james.apache.org/server/rfclist/smtp/rfc0821.txt
	  *
	  *	We must make sure there is no [.CRLF] characters sequence appended. If so, follow the RFC recommendations
	  */

	char *end = p + len, *ckpt = p;

	while (p != end)
	{
		if (*p == '\r' && p[1] == '\n' && p[2] == '.')
		{
			out->Append(ckpt, p - ckpt);
			out->Append("\r\n..", 4);

			p+=3;
			ckpt = p;
			continue;
		}
		p++;
	}

	out->Append(ckpt, p - ckpt);

}


bool SwitchMail::BuildMessageBody(CIOBuffer &buffer, CSwitchVector<email_t *, DeleteDestructor> *emailsFrom, CSwitchVector<email_t *, DeleteDestructor> *emailsTo, const char *subject, CBuffer *body, const char *cType, CXmlrpcStruct *opts, struct SwitchMail::email_validator_ctx *vc)
{
	DEFINE_STACKFRAME_SENTINEL;

	bool ignoreHeader;
	char *cdisp 	= NULL; 	/* Content disposition */
	contenttransferencoding_t cte;
	IXmlrpcValue *iv;

	if (Text::IsEmptyString(cType))
	{
		/* No content type specified. Assume default */

		cType = (char *)SWITCHMAIL_DEFAULT_CONTENTTYPE;
	}


	/* Initial headers .
	 * You may see that the optional 'no_from_header' and 'no_to_headers' allow for ignoring the standard From:/To: headers
	 * so that we an use our own or none. This allows, for example, sending (to) thousands of addressed and setting _To: as 'Club Members' and no_to_header to false, serving more like a list of users.
	 */


	ignoreHeader = false;
	if (opts && (iv = opts->Find("no_from_header", 14)) && iv->GetType() == IXmlrpcValue::XMLRPC_BOOLEAN)
		ignoreHeader = (bool)*(CXmlrpcBoolean *)iv;
	if (!ignoreHeader && emailsFrom && emailsFrom->GetSize())
	{
		buffer.Append("From: ", 6);
		OutputEmailsIntoBuffer(emailsFrom, &buffer);
		buffer.Append("\r\n", 2);
	}


	ignoreHeader = false;
	if (opts && (iv = opts->Find("no_to_header", 12)) && iv->GetType() == IXmlrpcValue::XMLRPC_BOOLEAN)
		ignoreHeader = (bool)*(CXmlrpcBoolean *)iv;
	if (!ignoreHeader && emailsTo && emailsTo->GetSize())
	{
		buffer.Append("To: ", 4);
		OutputEmailsIntoBuffer(emailsTo, &buffer);	
		buffer.Append("\r\n", 2);
	}



	if (likely(subject && *subject))
	{
		if (opts && (iv = opts->Find("subject_enc", 11)) && iv->GetType() == IXmlrpcValue::XMLRPC_STRING)
		{
			CXmlrpcString *e = (CXmlrpcString *)iv;

			if (ShouldEncodeHeaderValue((uint8_t *)subject))
			{
				buffer.AppendFmt("Subject: =?%.*s?B?", (uint32_t)e->GetLength(), e->GetData());
				EncodeBase64((uint8_t *)subject, Strlen(subject), &buffer, true);
				buffer.Append("?=\r\n", 4);
			}
			else
			{
				OutputHeaderValueIntoBuffer(buffer, "Subject: =?%.*s?%s", e->GetLength(), e->GetData(), subject);
			}

		}
		else if (ShouldEncodeHeaderValue((uint8_t *)subject))
		{
			buffer.Append("Subject: =?iso-8859-7?B?");
			EncodeBase64((uint8_t *)subject, Strlen(subject), &buffer, true);
			buffer.Append("?=\r\n", 4);
		}
		else
		{
			OutputHeaderValueIntoBuffer(buffer, "Subject: %s", subject);
		}
	}

	buffer.Append("X-Mailer: Phaistos Networks SMTP service\r\n");


	bool gotDate = false;
	if (opts)
	{
		if ((iv = opts->Find("headers", 7)) && iv->GetType() == IXmlrpcValue::XMLRPC_STRUCT)
		{
			CXmlrpcStruct *headers = (CXmlrpcStruct *)iv;
			CBuffer *hVal;
			CXmlrpcStruct::value_list_t *vl;


			for (vl = headers->members; vl; vl = vl->next)
			{

				hVal = (CBuffer *)(CXmlrpcString *)vl->value;

				/* We even allow for custom FROM/TO headers */

				if (!Strcasecmp(vl->name, "BCC"))
				{
					/* BCC is IGNORED ! blind carbon copy. 
					 * Used only when deciding where to dspatch an e-mail to.
					 */

					 continue;
				}

				if (vl->name[0] == '_')
				{
					/* Special treatment for those. Ignore '_' and use raw data */

					if (ShouldEncodeHeaderValue((uint8_t *)hVal->GetData()))
					{
						buffer.AppendFmt("%s: =?iso-8859-7?B?", (vl->name)+1);
						EncodeBase64(hVal, &buffer, true);
						buffer.Append("?=\r\n", 4);
					}
					else
					{
						OutputHeaderValueIntoBuffer(buffer, "%s: %s", (vl->name)+1, hVal->GetData());
					}
				}
				else if (!Strcasecmp(vl->name, "CC") || !Strcasecmp(vl->name, "BCC") || !Strcasecmp(vl->name, "REPLY-TO") || !Strcasecmp(vl->name, "FROM") || !Strcasecmp(vl->name, "TO"))
				{
					CSwitchVector<email_t *, DeleteDestructor> emails;

					ExtractEmails(hVal->GetData(), 0, &emails);

					if (vc)
						ValidateEmailsV(&emails, vc);

					buffer.AppendFmt("%s: ", vl->name);
					OutputEmailsIntoBuffer(&emails, &buffer);
					buffer.Append("\r\n", 2);
				}
				else
				{
					if (!gotDate && !Strcasecmp(vl->name, "Date"))
						gotDate = true;

					if (ShouldEncodeHeaderValue((uint8_t *)hVal->GetData()))
					{
						buffer.AppendFmt("%s: =?iso-8859-7?B?", vl->name);
						EncodeBase64(hVal, &buffer, true);
						buffer.Append("?=\r\n", 4);
					}
					else
					{
						OutputHeaderValueIntoBuffer(buffer, "%s: %s", vl->name, hVal->GetData());
					}
				}
			}
		}

		if ((iv = opts->Find("content-disposition", 19)) && iv->GetType() == IXmlrpcValue::XMLRPC_STRING)
			cdisp = (char *)*(CXmlrpcString *)iv;
	}

	if (!gotDate)
	{
		/* Date was not provided. Provide Date header for now 
		  Example:
		  Date: Wed, 20 Apr 2005 15:20:45 -0400
		 */

		buffer.Append("Date: ");

		struct tm tm;
		time_t now = time(NULL);

		localtime_r(&now, &tm);
		buffer.AppendFmt("%s, %u %s %u %02u:%02u:%02u +0200\r\n", Date::dayShortNames[tm.tm_wday], tm.tm_mday, Date::monShortNames[tm.tm_mon], tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}
	


	VALIDATE_STACKFRAME;


	/* Data depend on wether we have attachments, or not */

	CXmlrpcArray *attachments = NULL;
	uint nAttachments 	= 0;
	char boundary[256] 	= "";

	if (opts && (iv = opts->Find("attachments", 11)) && iv->GetType() == IXmlrpcValue::XMLRPC_ARRAY)
	{
		/* Each attachment is a structure with properties. Essential / required are raw_data ( base64 or string ) */

		attachments = (CXmlrpcArray *)iv;
		CXmlrpcStruct *attachment;
		bool isRaw;


		for (uint i = 0; i < attachments->GetSize(); i++)
		{
			if ((iv = attachments->GetElement(i)) && iv->GetType() == IXmlrpcValue::XMLRPC_STRUCT)
			{
				attachment = (CXmlrpcStruct *)iv;

		
				isRaw = false;
				if ((iv = attachment->Find("is_raw", 6)) && iv->GetType() == IXmlrpcValue::XMLRPC_BOOLEAN && *(bool *)iv)
				{
					/* This is a RAW attachment. Store data as-is */

					isRaw = true;
				}


				char *atCtype = NULL;

				if ((iv = attachment->Find("content_type", 12)) && iv->GetType() == IXmlrpcValue::XMLRPC_STRING)
					atCtype = (char *)*(CXmlrpcString *)iv;
				

				iv = NULL;
				if ((atCtype && !Strncasecmp(atCtype, "multipart/", 10)) || ((iv = attachment->Find("data")) && (iv->GetType() == IXmlrpcValue::XMLRPC_STRING || iv->GetType() == IXmlrpcValue::XMLRPC_BASE64)))
				{
					
					if (++nAttachments == 1)
					{
						/* Build boundary */

						Snprintf(boundary, sizeof(boundary), "--_PhaistosMailMsgPart_%d_%d_%d__", (int)time(NULL), getpid(), (int)time(NULL) % 8192);


						/* Standard multiplart headers */

						buffer.AppendFmt("Content-Type: multipart/mixed;\r\n\tboundary=\"%s\"\r\n", boundary);
						buffer.Append("MIME-Version: 1.0\r\n");
						
						buffer.Append("\r\n", 2); 	/* End of headers */


						/* First messsage */


						cte = GuessContentTransferEncoding(body, cType);

						if (cte == CTE_UNKOWN)
						{
							cte = CTE_BINARY;
							cType = (char *)SWITCHMAIL_DEFAULT_CTYPEFORUNKNOWNCTE;
						}


						buffer.Append("This is a multi-part message in MIME format.\r\n");
						buffer.AppendFmt("--%s\r\n", boundary);
						buffer.AppendFmt("Content-Type: %s\r\n", cType);
						buffer.AppendFmt("Content-Transfer-Encoding: %s\r\n", GetTransferEncodingName(cte));
						if (cdisp)
							buffer.AppendFmt("Content-Disposition: %s\r\n", cdisp);

						/* End of headers */
						buffer.Append("\r\n", 2);

						switch (cte)
						{
							case CTE_7BIT:
							case CTE_8BIT:
							case CTE_BINARY:
								AppendTextCarefully(body->GetData(), body->GetLength(), &buffer);
								break;

							case CTE_QUOTED_PRINTABLE:
								EncodeQuotedPrintable(body, &buffer);
								break;

							case CTE_BASE64:
								EncodeBase64(body, &buffer);
								break;

							case SwitchMail::CTE_UNKOWN:
								break;
						}

							
						buffer.Append("\r\n", 2);

					}


					/* Dump attachment */

					
					char  *atCDisposition = NULL, *atName = NULL, *atCid = NULL;
					bool alreadyEncoded = false;
					CBuffer *atData = NULL;

					if ((iv = attachment->Find("data", 4)))
					{
						if (iv->GetType() == IXmlrpcValue::XMLRPC_STRING)
							atData = (CBuffer *)(CXmlrpcString *)iv;
						else
							atData = (CBuffer *)(CXmlrpcBase64 *)iv;
					}


					if (isRaw)
					{
						buffer.AppendFmt("--%s\r\n", boundary); 	/* Mark beginning of a new attachment */

						if (atData)
							buffer.Append(*atData);
					}
					else
					{
						cte = CTE_UNKOWN;
						if ((iv = attachment->Find("encoding", 8)) && iv->GetType() == IXmlrpcValue::XMLRPC_STRING)
						{
							/* Already encoded data */
						
							alreadyEncoded = true;
							cte = GetTransferEncodingType((char *)*(CXmlrpcString *)iv);
						}

						if ((iv = attachment->Find("content_disposition", 19)) && iv->GetType() == IXmlrpcValue::XMLRPC_STRING)
							atCDisposition = (char *)*(CXmlrpcString *)iv;
						

						if ((iv = attachment->Find("name", 4)) && iv->GetType() == IXmlrpcValue::XMLRPC_STRING)
							atName = (char *)*(CXmlrpcString *)iv;

						
						if ((iv = attachment->Find("cid", 3)) && iv->GetType() == IXmlrpcValue::XMLRPC_STRING)
							atCid= (char *)*(CXmlrpcString *)iv;


					
						buffer.AppendFmt("--%s\r\n", boundary); 	/* Mark beginning of a new attachment */


						if (!alreadyEncoded)
							cte = GuessContentTransferEncoding(atData, atCtype);

						if (cte == CTE_UNKOWN)
						{
							cte = CTE_BINARY;
							atCtype = (char *)SWITCHMAIL_DEFAULT_CTYPEFORUNKNOWNCTE;
						}


						if (atCtype && atName)
						{
							if (ShouldEncodeHeaderValue((uint8_t *)atName))
							{
								buffer.AppendFmt("Content-Type: %s; name=\"=?iso-8859-7?B?", atCtype);
								EncodeBase64((uint8_t *)atName, Strlen(atName), &buffer, true); 
								buffer.Append("?=\"\r\n");

							}
							else
								OutputHeaderValueIntoBuffer(buffer, "Content-Type: %s; name=\"%s\"", atCtype, atName);
						}
						else if (atCtype)
							OutputHeaderValueIntoBuffer(buffer, "Content-Type: %s", atCtype);

						buffer.AppendFmt("Content-Transfer-Encoding: %s\r\n", GetTransferEncodingName(cte));
						if (atCDisposition && atName)
						{

							if (ShouldEncodeHeaderValue((uint8_t *)atName))
							{
								buffer.AppendFmt("Content-Disposition: %s; filename=\"=?iso-8859-7?B?", atCDisposition);
								EncodeBase64((uint8_t *)atName, Strlen(atName), &buffer, true); 
								buffer.Append("?=\"\r\n");

							}
							else
								OutputHeaderValueIntoBuffer(buffer, "Content-Disposition: %s; filename=\"%s\"", atCDisposition, atName);
						}
						else if (atCDisposition)
							OutputHeaderValueIntoBuffer(buffer, "Content-Disposition: %s", atCDisposition);
						if (atCid)
							OutputHeaderValueIntoBuffer(buffer, "Content-Id: %s", atCid);

						


						buffer.Append("\r\n", 2); 		/* End attachment headers */


						if (atData)
						{
							if (alreadyEncoded)
							{
								/* Already encoded data provided */

								buffer.Append(atData->GetData(), atData->GetLength());
							}
							else
							{
								/* Dump attachment data */
								
								switch (cte)
								{
									case CTE_7BIT:
									case CTE_8BIT:
									case CTE_BINARY:
										AppendTextCarefully(atData->GetData(), atData->GetLength(), &buffer);
										break;

									case CTE_QUOTED_PRINTABLE:
										EncodeQuotedPrintable(atData, &buffer);
										break;

									case CTE_BASE64:
										EncodeBase64(atData, &buffer);
										break;

									case CTE_UNKOWN:
										break;
								}
							}
						}


						buffer.Append("\r\n", 2);

					} /* Not raw */

				}
				else
				{
					SwitchDebug("Skipped attachment (content type = %s)\n", atCtype ? atCtype : (char *)"N/A");
				}
			}
		}
	}


	VALIDATE_STACKFRAME;

	if (!nAttachments)
	{
		/* No attachments; the body alone will do */

		cte = GuessContentTransferEncoding(body, cType);

		if (cte == CTE_UNKOWN)
		{
			cte = CTE_BINARY;
			cType = (char *)SWITCHMAIL_DEFAULT_CTYPEFORUNKNOWNCTE;
		}

		buffer.AppendFmt("Content-type: %s\r\n", cType);
		buffer.AppendFmt("Content-Transfer-Encoding: %s\r\n", GetTransferEncodingName(cte));
		if (cdisp)
			buffer.AppendFmt("Content-Disposition: %s\r\n", cdisp);

		buffer.Append("\r\n", 2); 	/* End of headers */

		switch (cte)
		{
			case CTE_7BIT:
			case CTE_8BIT:
			case CTE_BINARY:
				AppendTextCarefully(body->GetData(), body->GetLength(), &buffer);
				break;

			case CTE_QUOTED_PRINTABLE:
				EncodeQuotedPrintable(body, &buffer);
				break;

			case CTE_BASE64:
				EncodeBase64(body, &buffer);
				break;

			case CTE_UNKOWN:
				break;
		}



		buffer.Append("\r\n", 2);
	}
	else
	{
		/* Mark the last attachment */

		buffer.AppendFmt("--%s--\r\n", boundary);
	}

	VALIDATE_STACKFRAME;


	return true;
}


static bool IsEmailValid(uint8_t *emBegin, uint8_t *emEnd)
{
	while (emBegin != emEnd)
	{
		switch (*(emBegin++))
		{
			case 0 ... 32:
				return false;

			case '(':
			case ')':
			case '>':
			case '<':
				return false;

			case 128 ... 255:
				return false;
		}
	}

	return true;
}


/* XXX: <email><email2> will extract email as email and email2 as its name!  
 * 
 *	TODO: Use crawler component TLDs check for well formed email addresses
 */

bool SwitchMail::ExtractEmails(const char *emailData, const uint32_t options, CSwitchVector<email_t *, DeleteDestructor> *emails)
{
	uint8_t *data =(uint8_t *)emailData, *p = NULL;
	uint8_t *nmBegin = NULL, *nmEnd = NULL, *emBegin = NULL, *emEnd = NULL, *potentialEnd;
	email_t *email;
	uint32_t len;

	while (*data)
	{

		if (*data == '\"')
		{
			/* Expected name here */

			nmBegin = ++data; 	/* Begining of name */
			while (*data && *data != '\"')
				data++;
			if (*data != '\"')
				return true;
			nmEnd = data; 		/* End of name */
			data++;

			if (emBegin && emEnd)
			{

				if (nmBegin)
				{
					if (!nmEnd)
						nmEnd = data;

					while (*nmBegin == ' ' || *nmBegin == '\t')
						nmBegin++;

					while (nmEnd > nmBegin && (*(nmEnd-1) == ' ' || *(nmEnd-1) == '\t'))
						nmEnd--;
				}

				if (IsEmailValid(emBegin, emEnd))
				{
					/* Register it here */

					email = new email_t();

					len = emEnd - emBegin;
					email->email = (char *)Malloc(len+1);
					Memcpy(email->email, emBegin, len);
					email->email[len] = '\0';

					if (!((options & EXTRACT_EMAILS_NO_NAME) == EXTRACT_EMAILS_NO_NAME) && nmBegin)
					{
						len = nmEnd - nmBegin;
						email->name = (char *)Malloc(len+1);
						Memcpy(email->name, nmBegin, len);
						email->name[len] = '\0';
					}
					emails->Add(email);
				}

#if defined(DBG_EXTRACTION)

				printf("EMAIL: [%s] name [%s]\n", email->email, email->name ? email->name : "N/A");
#endif
			
				emBegin = emEnd = nmEnd = nmBegin = NULL;
			}
		
			continue;
		}

		if (*data == ',' || *data == ';')
		{

			if (emBegin && emEnd)
			{
				if (nmBegin)
				{
					if (!nmEnd)
						nmEnd = data;

					while (*nmBegin == ' ' || *nmBegin == '\t')
						nmBegin++;

					while (nmEnd > nmBegin && (*(nmEnd-1) == ' ' || *(nmEnd-1) == '\t'))
						nmEnd--;
				}

				/* Register it here */

				if (IsEmailValid(emBegin, emEnd))
				{
					email = new email_t();

					len = emEnd - emBegin;
					email->email = (char *)Malloc(len+1);
					Memcpy(email->email, emBegin, len);
					email->email[len] = '\0';

					if (!((options & EXTRACT_EMAILS_NO_NAME) == EXTRACT_EMAILS_NO_NAME) && nmBegin)
					{
						len = nmEnd - nmBegin;
						email->name = (char *)Malloc(len+1);
						Memcpy(email->name, nmBegin, len);
						email->name[len] = '\0';
					}
					emails->Add(email);
				}

#if defined(DBG_EXTRACTION)
				printf("EMAIL: [%s] name [%s]\n", email->email, email->name ? email->name : "N/A");
#endif

			


			}

			nmBegin = nmEnd = emBegin = emEnd = NULL;
		}

		/* Got ordinary data here.. Is it our e-mail address? */

		if (*data == '>')
		{
			/* Probably, beginning or ending of an address - we ignore it anyway */

			data++;
			continue;
		}

		if (*data!=','  && *data != ';' && data != (uint8_t *)emailData && *data != ' ' &&  *data != '<' &&  *data != '\t')
		{
			data++;
			continue;
		}

		potentialEnd = data;

		if (*data == ' ' || *data == '\t')
		{
			while (*data == ' ' || *data == '\t')
				data++;

			if (*data == ',' || *data == ';')
				continue;
		}
		if (*data == '<' || *data == ',' || *data == ';')
		{
			data++;
			while (*data == ' ' || *data == '\t')
				data++;
		}

		if (*data == '\"' || *data == ',' || *data == ';' || *data == '<')
			continue;
	

		if (!emBegin)
		{
			bool emailFlag1 = false, emailFlag2 = false;

			for (p = data; *p && *p !=',' && *p!=';' && *p != ' ' && *p != '\t' && *p !='>'; p++)
			{
				switch (*p)
				{	
					case '@':
						emailFlag1 = true;
						break;

					case '.':
						if (emailFlag1)
							emailFlag2 = true;
						break;
				}
			}

			if (emailFlag1 && emailFlag2)
			{
				/* We got our email address */

				emBegin = data;
				emEnd 	= p;

				if (emBegin && emEnd)
				{
					if (nmBegin)
					{
						if (!nmEnd)
							nmEnd = potentialEnd;

						while (*nmBegin == ' ' || *nmBegin == '\t')
							nmBegin++;

						while (nmEnd > nmBegin && (*(nmEnd-1) == ' ' || *(nmEnd-1) == '\t'))
							nmEnd--;
					}

				}

	
				data = p;

				ASSERT_VALID_POINTER(data);
				continue;

			}
		}


		if (!nmBegin)
		{
			nmBegin = data;
		}

		if (data == (uint8_t *)emailData)
			data++;


	}


	if (emBegin && emEnd)
	{
		if (nmBegin)
		{
			if (!nmEnd)
				nmEnd = data;

			while (*nmBegin == ' ' || *nmBegin == '\t')
				nmBegin++;

			while (nmEnd > nmBegin && (*(nmEnd-1) == ' ' || *(nmEnd-1) == '\t'))
				nmEnd--;
		}

		/* Register it here */
	
		if (IsEmailValid(emBegin, emEnd))
		{
			email = new email_t();

			len = emEnd - emBegin;
			email->email = (char *)Malloc(len+1);
			Memcpy(email->email, emBegin, len);
			email->email[len] = '\0';

			if (!((options & EXTRACT_EMAILS_NO_NAME) == EXTRACT_EMAILS_NO_NAME) && nmBegin)
			{
				len = nmEnd - nmBegin;
				email->name = (char *)Malloc(len+1);
				Memcpy(email->name, nmBegin, len);
				email->name[len] = '\0';
			}
			emails->Add(email);
		}

		
#if defined(DBG_EXTRACTION)
		printf("EMAIL: [%s] name [%s]\n", email->email, email->name ? email->name : "N/A");
#endif


	}


	return true;
}


//#define DEBUG_VALIDATOR

int SwitchMail::ValidateEmail(SwitchMail::email_t *email, struct SwitchMail::email_validator_ctx *ctx)
{
	if (unlikely(!email || !email->email))
	{
#ifdef DEBUG_VALIDATOR
		SwitchDebug("No email provided\n");
#endif
		return -1;
	}


	const char *address 	= email->email;
	uint32_t len 		= Strlen(address);
	const uint32_t _len 	= len;
	CCrawler::tld_t *tld;

	while (--len && address[len] != '.')
		continue;

	tld = CCrawler::GetTldFromString(address + len + 1);

#ifdef DEBUG_VALIDATOR
	SwitchDebug("Email [%s] TLD:%s\n", address, address + len + 1);
#endif

	if (!tld)
	{
#ifdef DEBUG_VALIDATOR
		SwitchDebug("Unknown TLD[%s]\n", address + len + 1);
#endif

		return -1;
	}

	while (--len && address[len] != '@')
		continue;
	
	const char *hostName 		= address + len + 1;
	const uint32_t hostNameLen 	= _len - len - 1;
	uint8_t v;

	if (!ctx->ht.FindValue(hostName, hostNameLen, v))
	{
		static const char *knownDomains[] = 
		{
#ifndef DEBUG_VALIDATOR
			"pathfinder.gr",
			"pblogs.gr",
			"unews.gr",
			"vodafone.gr",
			"vodafone.com",
			"adman.gr",
			"phaistosnetworks.gr",
			"gmail.com",
			"yahoo.com",
			"google.com",
			"hotmail.com",
			"facebook.com",
			"flickr.com",
			"in.gr",
			"yahoo.gr"
#endif
		};
		const size_t nKnownDomains = ARRAY_SIZE(knownDomains);
		uint32_t i;

		for (i = 0; i < nKnownDomains; ++i)
		{
			if (!strcasecmp(hostName, knownDomains[i]))
			{
#ifdef DEBUG_VALIDATOR
				SwitchDebug("Known domain[%.*s]\n", hostNameLen, hostName);
#endif
				v = email_validator_ctx::VALIDATED;
				break;
			}
		}

		if (i == nKnownDomains)
		{
#ifdef DEBUG_VALIDATOR
			printf("Resolve %s\n", hostName);
#endif

			CSwitchFastDns::qanswer_t *answer = ctx->resolver.ResolveByNameEx(hostName, CSwitchFastDns::RR_TYPE_MX, ctx->buf, 2, sizeof(ctx->buf) - 1);

			if (!answer)
			{
				v = email_validator_ctx::UNKNOWN_SMTP_HOST;
#ifdef DEBUG_VALIDATOR
				SwitchDebug("Failed to resolve [%.*s]\n", hostNameLen, hostName);
#endif

			}
			else
			{
				CSwitchFastDns::qresp_mx_t *mxRecord;

#ifdef DEBUG_VALIDATOR
				printf("Got answer(s)\n");
#endif

				v = email_validator_ctx::NO_MX;
				while (answer)
				{
					if (unlikely(answer->rType != CSwitchFastDns::RR_TYPE_MX))
					{
						answer = answer->next;
						continue;
					}

					mxRecord = (CSwitchFastDns::qresp_mx_t *)answer->rVal;
#ifdef DEBUG_VALIDATOR
					SwitchDebug("%u:%s\n", mxRecord->prio, mxRecord->mailExchange);
#endif
					const uint32_t ipAddress = ctx->resolver.ResolveIP4(mxRecord->mailExchange, 2);

					if (ipAddress != INADDR_NONE)
					{
						v = email_validator_ctx::VALIDATED;
#ifdef DEBUG_VALIDATOR
						SwitchDebug("%s resolved\n", mxRecord->mailExchange);
#else
						break;
#endif

					}

					answer = answer->next;
				}

			}
		}

#ifdef DEBUG_VALIDATOR
		printf("Registered host\n");
#endif

		ctx->ht.Add(hostName, hostNameLen, v);
	}

	
	return v;
}



void SwitchMail::ValidateEmailsV(CSwitchVector<SwitchMail::email_t *, DeleteDestructor> *v, struct SwitchMail::email_validator_ctx *ctx)
{
	//return; 	/* XXX: don't bother until whatever bug associated with CSwitchFastDns is fixed */

	for (uint32_t i = 0; i < v->GetSize(); )
	{
		if (ValidateEmail(v->GetElement(i), ctx) != email_validator_ctx::VALIDATED)
		{
			delete v->PopByIndex(i);
			continue;
		}

		++i;
	}
}


void SwitchMail::OutputHeaderValueIntoBuffer(CBuffer &buffer, const char *fmt, ...)
{
	va_list args;
	int l, capacity = 256;

	char *data = (char *)Malloc(capacity);
	va_start(args, fmt);
	l = vsnprintf(data, capacity, fmt, args);
	if (l >= capacity)
	{
		capacity = l + 1;
		Free(data);
		data = (char *)Malloc(capacity);
		vsnprintf(data, capacity, fmt, args);
	}
	va_end(args);

	char *origData = data;
	uint i;

	
	do
	{
		for (i = 0; data[i] && i < LINELEN; i++)
			continue;
		if (i)
		{
			buffer.Append(data, i);

			if (i == LINELEN && data[i])
			{
				/* There is more to it -- break down the line */

				buffer.Append("\r\n\t", 3);
			}
		}

		if (!(*data))
			break;

		data+=i;
	} while (*data);

	buffer.Append("\r\n", 2);
	Free(origData);
}


void SwitchMail::OutputEmailsIntoBuffer(CSwitchVector<email_t *, DeleteDestructor> *emails, CBuffer *buffer)
{
	uint i, k, nOut = 0;
	email_t *email;
	int lastNlPos = buffer->GetLength();

	for (i = 0; i < emails -> GetSize(); i++)
	{
		for (email = emails->GetElement(i), k = 0; k < i; k++)
		{
			if (!Strcasecmp(email->email, emails->GetElement(k)->email))
				break;
		}

		if (k != i)
			continue;

		if (nOut++)
			buffer->Append(", ", 2);

		if (buffer->GetLength() - lastNlPos > LINELEN)
		{
			/* Mind the LINELEN characters limit */

			buffer->Append("\r\n\t", 3);
			lastNlPos = buffer->GetLength();
		}


		if (email->name)
		{
			if (ShouldEncodeHeaderValue((uint8_t *)email->name))
			{
				buffer->Append("\"=?iso-8859-7?B?");
				EncodeBase64((uint8_t *)email->name, Strlen(email->name), buffer, true);
				buffer->Append("?=\" ", 4);

			}
			else
				buffer->AppendFmt("\"%s\" ", email->name);
		}

		buffer->AppendFmt("<%s>", email->email);
	}

}


int SwitchMail::DecodeBase64(const uint8_t *data, size_t len, CBuffer *output)
{
	int required = len + 16;

	if (output->GetSize() - output->GetLength() <= (unsigned)required)
		output->Expand(required);

	if ((required = DecodeBase64(data, len, (uint8_t *)output->GetData() + output->GetLength())) > 0)
		output->SetLength(output->GetLength() + required);

	return required;
}


//#define DECODE_FAILURE() SwitchDebug("Failed to decode: %s", data);
#define DECODE_FAILURE()

int SwitchMail::DecodeBase64(const uint8_t *data, size_t len, uint8_t *output)
{
	//static const char base64Code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	uint8_t dtable[256];
	uint i;

	memset(&dtable[0], 0x80, sizeof(dtable));

	for (i = 'A'; i <= 'I'; i++)
		dtable[i] = 0 + (i - 'A');
	for (i = 'J'; i <= 'R'; i++)
		dtable[i] = 9 + (i - 'J');
	for (i = 'S'; i <= 'Z'; i++)
		dtable[i] = 18 + (i - 'S');
	for (i = 'a'; i <= 'i'; i++)
		dtable[i] = 26 + (i - 'a');
	for (i = 'j'; i <= 'r'; i++)
		dtable[i] = 35 + (i - 'j');                                                                                 
	for (i = 's'; i <= 'z'; i++)                                                                                    
		dtable[i] = 44 + (i - 's');                                                                                 
	for (i = '0'; i <= '9'; i++)                                                                                    
		dtable[i] = 52 + (i - '0');                                                                                 
	dtable[(int)'+'] = 62;                                                                                               
	dtable[(int)'/'] = 63;                                                                                               
	dtable[(int)'='] = 0;  

	uint8_t a, b, c, d;
	uint8_t cC, dC;
	uint8_t *origOutput = output;
	const uint8_t *endOfData = data + len;

	while (data < endOfData)
	{


		while (*data < ' ' && data < endOfData) data++;
		if (data >= endOfData) 			/* Extra care required here */
			break;
		if ((a = dtable[*data++]) & 0x80)
		{
			DECODE_FAILURE();
			return -1;
		}

		while (*data < ' ' && data < endOfData) data++;
		if (data >= endOfData) 			/* Extra care required here */
			break;
		if ((b = dtable[*data++]) & 0x80)
		{
			DECODE_FAILURE();
			return -1;
		}


		while (*data < ' ' && data < endOfData) data++;
		if (data >= endOfData) 			/* Extra care required here */
			break;
		if ((c = dtable[(cC = *(data++))]) & 0x80)
		{
			DECODE_FAILURE();
			return -1;
		}


		while (*data < ' ' && data < endOfData) data++;
		if (data >= endOfData) 			/* Extra care required here */
			break;
		if ((d = dtable[(dC = *(data++))]) & 0x80)
		{
			DECODE_FAILURE();
			return -1;
		}


		if (cC == '=')
			*output++ = (a << 2) | (b >> 4);
		else if (dC == '=')
		{
			*output++ = (a << 2) | (b >> 4);
			*output++ = (b << 4) | (c >> 2);
		}
		else
		{
			*output++ = (a << 2) | (b >> 4);
			*output++ = (b << 4) | (c >> 2);
			*output++ = (c << 6) | d;
		}
	}


	*output = '\0';

	return output - origOutput;
}



inline static int Hex2Int(const uint8_t v)
{
	switch (v)
	{
		case '0' ... '9':
			return v - '0';

		case 'A' ... 'F':
			return v - 'A' + 10;

		case 'a' ... 'f':
			return v - 'a' + 10;
	}

	return 0; /* Keep compiler happy */ 
}



int SwitchMail::DecodeQuotedPrintable(const uint8_t *data, size_t len, uint8_t *output)
{
	
	uint8_t *origOutput = output;
	const uint8_t *endOfData = data + len;



	while (data < endOfData)
	{
		switch (*data)
		{
			case '=':
				if (isxdigit(*(data+1)) && isxdigit(*(data+2)))
				{
					*output++ = (Hex2Int(*(data+1)) << 4) + Hex2Int(*(data+2));
					data+=3;
				}
				else
				{
					/* Check for soft line breaks / RFC 2045 */

					data++;
					while (*data == ' ' || *data == '\t')
						data++;


					if (*data == '\r' && *(data+1) == '\n')
						data+=2;
					else if (*data == '\r' || *data == '\n')
						data++;
					else if (*data == '\0')
						break;
					else
						*output++ =*data++;
				}
				break;

			default:
				*output++  = *data++;
				break;
		}

	}

	*output = '\0';
	return output - origOutput;
}

int SwitchMail::DecodeQuotedPrintable(const uint8_t *data, size_t len, CBuffer *output)
{
	uint required = len + 16;

	if (output->GetSize() - output->GetLength() <= required)
		output->Expand(required);
	if ((required = DecodeQuotedPrintable(data, len, (uint8_t *)output->GetData() + output->GetLength())))
		output->SetLength(output->GetLength() + required);
	
	return required;
}


int SwitchMail::DecodeHeader(const uint8_t *data, CBuffer *output, char *charset)
{

	/*
	 *	Subject: =?ISO-8859-7?B?SSB0aG91Z2h0IEkgdGhhdyDd7eEg4evv49zq6Q==?=
	 */

	const uint8_t *end, *origData = data;
	const uint8_t *plainDataStart = NULL;

	if (charset)
		*charset = '\0';


	do
	{
		if (*data == '=' && data[1] == '?')
		{
			/* Encoded - Retrieve character set */

			if (plainDataStart)
			{
				output->Append((char *)plainDataStart, data - plainDataStart);
				plainDataStart = NULL;
			}


			const uint8_t *checkpoint = data + 2;

			for (data = checkpoint; ;++data)
			{
				if (*data == '?')
				{
					if (charset)
					{
						const size_t charsetLen = data - checkpoint;

						if (likely(charsetLen < 64))
						{
							memcpy(charset, checkpoint, charsetLen);
							charset[charsetLen] = '\0';
							printf("charset:[%s]\n", charset);
						}	
					}

					++data;
					break;
				}
				else if (unlikely(*data == '\0'))
					return -1;
			}

			switch (*data)
			{
				case 'b':
				case 'B':
					/* Base64 encoded */
					data+=2;
					for (end = data; *end; end++)
					{
						if (*end == '?' && *(end+1) == '=')
							break;
					}
					if (*end != '?')
					{
						/* Screwed header */

						return -1;
					}
					if (DecodeBase64(data, end - data, output) == -1)
					{
						/* Unable to decode data */

						return -1;
					}
					data = end + 2;
					break;

				case 'q':
				case 'Q':
					/* Quoted printable, encoded */
					data+=2;
					for (end = data; *end; end++)
					{
						if (*end == '?' && *(end+1) == '=')
							break;
					}
					if (*end != '?')
					{
						/* Screwed header */

						return -1;
					}
					if (DecodeQuotedPrintable(data, end - data, output) == -1)
					{
						/* Unable to decode data */

						return -1;
					}
					data = end + 2;
					break;


				default:
					/* Unknown encoding method */
					return -1; 
			}

		}				
		else if (*data == '\r' && *(data+1) == '\n')
		{

			if (plainDataStart)
			{
				output->Append((char *)plainDataStart, data - plainDataStart);
				plainDataStart = NULL;
			}

			if (*(data+2) == ' ' || *(data+2) == '\t')
			{
				/* More to follow */

				data += 3;
			}
			else
			{
				/* End of data */
				data+=2;
				break;
			}
		}
		else if (*data == '\n')
		{
			if (plainDataStart)
			{
				output->Append((char *)plainDataStart, data - plainDataStart);
				plainDataStart = NULL;
			}

			if (*(data+1) == ' ' || *(data+1) == '\t')
			{
				/* More to follow */

				data += 2;
			}
			else
			{

				/* End of data */
				data++;
				break;
			}


		}
		else if (*data)
		{
			if (!plainDataStart)
				plainDataStart = data;

			data++;
		}


	} while (*data);

	if (plainDataStart)
		output->Append((char *)plainDataStart, data - plainDataStart);


	return data - origData;
}




static inline void SkipRn(char *&b)
{
	b+= (*b == '\n' ? 1 : 2);
}

int SwitchMail::ParsePart(CBuffer *msgData, uint dataPos, uint partLength, int parent, CSwitchVector<email_part_t *, DeleteDestructor> *parts)
{
	uint8_t *headerNameBegin, *headerNameEnd;
	CBuffer hVal;
	int hvalDataLen;

	/* Prepare the email part info */

	email_part_t *part = new email_part_t();
	part->cte          = CTE_UNKOWN; 	/* Initially, no content type is defined */
	part->headersStart = dataPos;
	part->endOfHeaders = 0;
	part->parentIndex  = parent;
	part->beginOfData  = 0;
	part->endOfData    = 0;
	parts->Add(part);

	uint thisPartIndex = parts->GetSize() - 1;


	uint32_t headerNameLen, boundaryLen = 0;
	uint8_t *b = (uint8_t *)msgData->GetData() + dataPos;
	char boundary[256] = "", *attrVal, q;



	/* Read in all headers 
	 * We may actually end up getting no headers at all 
	 */

	for (;;)
	{

		if (*b == '\r' && b[1] == '\n')
		{
			/* No more headers to left - for the body */

			part->endOfHeaders = b - (uint8_t *)msgData->GetData();
			b+=2;
			break;
		}
		else if (*b == '\n')
		{
			/* More headers to check #2 */

			part->endOfHeaders = b - (uint8_t *)msgData->GetData();
			b++;
			break;
		}


		for (headerNameBegin = b; likely(*b) && *b != ' ' && *b != '\r' && *b != '\n' && *b != ':'; ++b)
			continue;

		if (unlikely(*b !=':'))
		{
			/* Malformed messages */

			SwitchDebug("Malformed ':'[%.256s]\n", b);
			return -1;
		}

		headerNameEnd 	= b;


		++b;
		if (*b == ' ' || *b == '\t')
		{
			/* Skip space/tab after ':' */

			++b;
		}





		hVal.Flush();


		/* Process common headers */

		headerNameLen = headerNameEnd - headerNameBegin;

	
		if (!Strncasecmp((char *)headerNameBegin, "Content-type", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);

			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;



			delete part->cType;
			part->cType = hVal.CloneString();

			if ((attrVal = GetHeaderAttr(&hVal, "boundary")))
			{	
				/* We got a boundary to look for within this text */

				boundaryLen = 0;
				boundary[boundaryLen++] = '-';
				boundary[boundaryLen++] = '-';

				q = 0;
				if (*attrVal == '\"' || *attrVal == '\'')
					q = *(attrVal++);
			
				for (; *attrVal  && boundaryLen < (int)sizeof(boundary) -1 ; attrVal++)
				{
					if (!q && (*attrVal  == ' ' || *attrVal == ';' || *attrVal == '\r' || *attrVal == '\t' || *attrVal == '\n'))
						break;
					else if (q && *attrVal == q)
						break;
					boundary[boundaryLen++] = *attrVal;
				}
				boundary[boundaryLen] = '\0';

				if (boundaryLen < 4)
				{
					/* Too short boundary */

					SwitchDebug("Too short boundary\n");
					return -1;
				}

			}
					
		}
		else if (!Strncasecmp((char *)headerNameBegin, "Content-Id", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;



			delete part->contentId;
			part->contentId = hVal.CloneString();
		}
		else if (!Strncasecmp((char *)headerNameBegin, "Content-Id", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;



			delete part->messageId;
			part->messageId = hVal.CloneString();
		}
		else if (!Strncasecmp((char *)headerNameBegin, "References", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;



			delete part->references;
			part->references = hVal.CloneString();
		}
		else if (!Strncasecmp((char *)headerNameBegin, "Reply-To", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;


			delete part->replyTo;
			part->replyTo= hVal.CloneString();
		}
		else if (!Strncasecmp((char *)headerNameBegin, "Date", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;


			delete part->date;
			part->date = hVal.CloneString();
		}
		else if (!Strncasecmp((char *)headerNameBegin, "Content-Transfer-Encoding", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;



			part->cte = GetTransferEncodingType(hVal.GetData());

		}
		else if (!Strncasecmp((char *)headerNameBegin, "Content-Disposition", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;



			delete part->cDisp;
			part->cDisp = hVal.CloneString();
		}
		else if (!Strncasecmp((char *)headerNameBegin, "Subject", headerNameLen))
		{
			char subjectCType[64];

			hvalDataLen = DecodeHeader(b, &hVal, subjectCType);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;


			delete part->subject;
			delete part->subjectContentType;
			part->subject 		= hVal.CloneString();
			part->subjectContentType= NULL;
			if (subjectCType[0])
				part->subjectContentType = new CString(subjectCType);
		}
		else if (!Strncasecmp((char *)headerNameBegin, "From", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;



			delete part->from;
			part->from = hVal.CloneString();
		}
		else if (!Strncasecmp((char *)headerNameBegin, "To", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;



			delete part->to;
			part->to = hVal.CloneString();
		}
		else if (!Strncasecmp((char *)headerNameBegin, "Cc", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;



			delete part->cc;
			part->cc = hVal.CloneString();
		}
		else if (!Strncasecmp((char *)headerNameBegin, "Bcc", headerNameLen))
		{
			hvalDataLen = DecodeHeader(b, &hVal);
			if (hvalDataLen == -1)
			{
				/* Unable to parse header data */
				SwitchDebug("Unable to parse header data\n");
				return -1;
			}
			b+=hvalDataLen;



			delete part->bcc;
			part->bcc = hVal.CloneString();
		}
		else
		{
			/* Skip this header */

			b+=SkipHeader(b);
		}
	}



	if (!part->cType)
	{
		/* Check RFC - if no content type is provided, we assume the default */

		part->cType = new CString((char *)SWITCHMAIL_DEFAULT_CTYPEFORUNKNOWNCTE);
	}

	/* Done with headers, check body */



	part->beginOfData =  b - (uint8_t *)msgData->GetData();
	char *pos;

	
	if (boundary[0])
	{
		/* We got boundaries here */

		pos = Strstr((char *)b, boundary);
		if (!pos)
		{
			/* Expected boundary, not there */

			SwitchDebug("Expected boundary, missing '%s'\n", boundary);
			return -1;
		}

		pos += boundaryLen;
		if (*pos == '-' && *(pos+1) == '-')
		{
			/* There are no parts whatesoever in here . This is odd */

			pos+=2;
			SkipRn(pos);
			
			b = (uint8_t *)pos;
		}
		else
		{

#if defined(HANDLE_MISSING_BOUNDARYEND)
			bool gotEnd = false;
#endif

			do
			{
				SkipRn(pos);

				/* Part begins here. We must find its ending */

				b = (uint8_t *)pos;
		
				
				pos = Strstr((char *)b, boundary);
				if (!pos)
				{

#if defined(HANDLE_MISSING_BOUNDARYEND)
					/* Expected boundary, not found. Probably end of mail but
					 * stupid mailer 'forgot' to setup the end of the boundary
					 */

					pos = msgData->GetData() + dataPos + partLength;
					gotEnd = true;
#else

					SwitchDebug("Expected boundary, not found.. missing\n");
					return -1;

#endif

				}


				/* Process the data chunk. The chunk to process will be considered a sibling of this
				 * message part we are currently processing.
				 */

				if (ParsePart(msgData, b - (uint8_t *)msgData->GetData(), pos - (char *)b, thisPartIndex, parts) == -1)
				{
					/* Failed to parse part */
					return -1;
				}

#if defined(HANDLE_MISSING_BOUNDARYEND)
				if (gotEnd)
				{
					/* Found premature end a while ago */

					break;
				}
#endif

				pos+=boundaryLen;

				if (*pos == '-'  && *(pos+1) == '-')
				{
					/* End of boundaries list */

					pos+=2;
					SkipRn(pos);
					b = (uint8_t *)pos;
					break;
				}

			} while (true);
		}


		/* We found the end of data */
		part->endOfData = b - (uint8_t *)msgData->GetData();
	}
	else if (!Strncasecmp(part->cType->GetData(), "message/", 8))
	{
		/* Found a mesage here. Grab it from here onwards.
		 * message/rfc822 or others parts contain a new message, as if it was a whole new message by itself
		 */

		part->endOfData = dataPos + partLength;

		if (ParsePart(msgData, 	part->beginOfData , part->endOfData - part->beginOfData,  thisPartIndex, parts) == -1)
		{
			/* Failed to parse message part */

			return -1;
		}
	}
	else
	{
		/* No boundaries here */

		part->endOfData = dataPos + partLength;

	}


	/* How many parts were found, including childs, within here */

	return parts->GetSize() - parent;
}


char *SwitchMail::GetHeaderAttr(CBuffer *hVal, const char *name)
{
	char *p = hVal->GetData(), *end = p  + hVal->GetLength(), q = 0;
	size_t l = Strlen(name);

	while (p < end)
	{
		if (*p == '\'' || *p == '\"')
		{
			if (!q)	
				q = *p;
			else if (q == *p)
				q = 0;
		}
		else if (!q && !Strncasecmp(p, name, l) && p[l] == '=')
			return p + l + 1;
		p++;
	}

	return NULL;			
}

bool SwitchMail::GetHeaderAttrValue(CString *value, const char *name, const uint8_t nameLen, CBuffer *out)
{
	/* Useful for extracting, say, charset from a Content-Type header value
	 * TODO: extract all name/value pairs into a vector instead
	 */

	if (unlikely(!value))
		return false;

	char *p = value->GetData(), *n, *v, c;
	size_t len;

	for (;;)
	{
		if (*p == ';')
		{
			for (++p; *p == ' ' || *p == '\t'; ++p)
				continue;

			/* Expected token=value here */

			for (n = p++; (*p >='a' && *p <='z') || (*p >= 'A' && *p <='Z') || *p == '_'; ++p)
				continue;


			if (*p != '=')
			{
				++p;
				if (*p == ';')
					++p;
				continue;
			}
			else
			{
				const size_t nl = p - n;

				++p; 	/* skip past '=' */
				if ((c = *p) == '\"' || (c = *p) == '\'')
				{
					for (v = ++p; *p != c; ++p)
					{
						if (unlikely(*p == '\0'))
							return false;
					}

					len = p - v;
					++p;
				}
				else
				{
					v = p++;
					for (;;)
					{
						if (*p == ';')
						{
							len = p - v;
							++p;
							break;
						}
						else if (*p == '\0')
						{
							len = p - v;
							break;
						}
						else if (isblank(*p))
						{
							/* Unexpected? */

							return false;
						}

						++p;
					}
				}

				if (nl == nameLen && !Strncasecmp(n, name, nameLen))
				{
					out->Append(v, len);
					return true;
				}
			}
		}
		else if (unlikely(*p == '\0'))
			return false;
		else 
			++p;
	}

	return false;
}
char *SwitchMail::GetHeaderAttr(CString *hVal, const char *name)
{
	char *p = hVal->GetData(), *end = p + hVal->GetLength(), q = 0;
	size_t l = Strlen(name);

	while (p < end)
	{
		if (*p == '\'' || *p == '\"')
		{
			if (!q)	
				q = *p;
			else if (q == *p)
				q = 0;
		}
		else if (!q && !Strncasecmp(p, name, l) && p[l] == '=')
			return p + l + 1;
		p++;
	}

	return NULL;			
}


/* 
 * 	This should be invoked on parts of type 'message/-ANY-' or the topmost(whole message)
 * 	only.
 */

int SwitchMail::GuessBodyPart(CSwitchVector<email_part_t *, DeleteDestructor> *parts, int parentIndex)
{

	int selected = -1, bestGuess = -1;
	email_part_t *part;
	const size_t nParts = parts->GetSize();

	/* Start from first sibling of the father (remember -1 is whole message so the first
	  part we have extracted is 0 - the first part )
	  */

	for (uint32_t i = parentIndex + 1; i < nParts; ++i)
	{
		part = parts->GetElement(i);

		if (part->parentIndex == parentIndex || (part->parentIndex == -1 && parentIndex == 0))
		{
			if (!Strncasecmp(part->cType->GetData(), "multipart/related",  17))
			{
				/* Related - fall over */

				selected =  -1;
				parentIndex = i;
				continue;
			}
			else if (!Strncasecmp(part->cType->GetData(), "multipart/alternative", 21))
			{
				/* Alternative form, fall over */

				selected =  -1;
				parentIndex = i;
				continue;
			}
			else if (selected == -1 && !Strncasecmp(part->cType->GetData(), "multipart/", 10))
			{
				/* Any other multipart. Fall over if we have nothing */

				if (selected == -1)
				{
					parentIndex = i;
					continue;
				}

			}
			else if (!Strncasecmp(part->cType->GetData(), "text/html", 9) && !GetHeaderAttr(part->cType, "name"))
			{
				/* HTML parts have higher priority over everything else */

				return i;
			}
			else if (!Strncasecmp(part->cType->GetData(), "text/plain", 10) && !GetHeaderAttr(part->cType, "name"))
			{
				/* This will be our best guess */

				if (bestGuess == -1)
					bestGuess = i;
			}

		}
		else
			break;
	}

	return selected == -1 ? bestGuess : selected;

}


int SwitchMail::SkipHeader(const uint8_t *data)
{
	const uint8_t *origData = data;

	while (*data)
	{
		if (*data == '\r' && data[1] == '\n')
		{
			data+=2;

			if (*data == ' ' || *data == '\t')
				continue;

			return data - origData;
		}
		else if (*data == '\n')
		{
			++data;

			if (*data == ' ' || *data == '\t')
				continue;

			return data - origData;
		}
		else
			data++;
	}

	return data - origData;
}


SwitchMail::contenttransferencoding_t SwitchMail::GetPartCte(CSwitchVector<SwitchMail::email_part_t *, DeleteDestructor> *parts,  SwitchMail::email_part_t *part)
{
	while (part && part->cte == SwitchMail::CTE_UNKOWN)
		part = part->parentIndex != -1 ? parts->GetElement(part->parentIndex) : NULL;

	return part ? part->cte : SwitchMail::CTE_UNKOWN;
}

#endif
