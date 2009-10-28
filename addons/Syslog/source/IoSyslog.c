/*metadoc Syslog copyright
	Jeremy Tregunna, 2005
*/

/*metadoc Syslog license
	Revised BSD Revised
*/

/*metadoc Syslog description
Provides access to a Unix system's system logger.
<p>
<pre>
logger = Syslog clone do(
	identity("SyslogTest")
	facility(facilityMap at("LOG_USER"))
	options(List append(optionsMap at("LOG_PID"), optionsMap at("LOG_CONS")))
	priority(priorityMap at("LOG_INFO"))
	open(facility, options)
	mask(List append(maskMap at("LOG_PRIMASK")))
	log(priority, "*** Merely a test ***")
	close
)
</pre>	

<p>
Note: This is partially tested. Please let me know of any problems you happen to stumble across, or if it could be better. --Jeremy Tregunna
<p>
*/

//metadoc Syslog category Server


#include "IoSyslog.h"
#include "IoState.h"
#include "IoList.h"
#include <syslog.h>

#if defined(unix) || defined(__APPLE__)
#include <sys/utsname.h>
#endif

#define DATA(self) ((IoSyslogData *)IoObject_dataPointer(self))

IoTag *IoSyslog_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Syslog");
	IoTag_state_(tag, state);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSyslog_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSyslog_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSyslog_rawClone);
	return tag;
}

void IoSyslog_mark(IoSyslog *self)
{
	IoObject_shouldMark(DATA(self)->priority);
	IoObject_shouldMark(DATA(self)->facility);
	IoObject_shouldMark(DATA(self)->ident);
	IoObject_shouldMark(DATA(self)->optionsMap);
	IoObject_shouldMark(DATA(self)->facilityMap);
	IoObject_shouldMark(DATA(self)->priorityMap);
	IoObject_shouldMark(DATA(self)->maskMap);
}

IoSyslog *IoSyslog_proto(void *state)
{
	IoSyslog *self = IoObject_new(state);

	IoObject_tag_(self, IoSyslog_newTag(state));
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoSyslogData)));

	DATA(self)->syslog_opened = 0;
	DATA(self)->priority = IONUMBER(LOG_INFO);
	DATA(self)->facility = IONUMBER(LOG_LOCAL0);
	DATA(self)->syslog_mask = 0;
	DATA(self)->ident = IOSYMBOL("");
	DATA(self)->optionsMap = IoMap_new(IOSTATE);
	DATA(self)->facilityMap = IoMap_new(IOSTATE);
	DATA(self)->priorityMap = IoMap_new(IOSTATE);
	DATA(self)->maskMap = IoMap_new(IOSTATE);

	IoState_registerProtoWithFunc_(state, self, IoSyslog_proto);

	{
		IoMethodTable methodTable[] = {
		{"open", IoSyslog_open},
		{"reopen", IoSyslog_reopen},
		{"isOpen", IoSyslog_isOpen},
		{"close", IoSyslog_close},
		{"identity", IoSyslog_identity},
		{"options", IoSyslog_options},
		{"optionsMap", IoSyslog_optionsMap},
		{"facility", IoSyslog_facility},
		{"facilityMap", IoSyslog_facilityMap},
		{"priority", IoSyslog_priority},
		{"priorityMap", IoSyslog_priorityMap},
		{"mask", IoSyslog_mask},
		{"maskMap", IoSyslog_maskMap},
		{"log", IoSyslog_log},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoSyslog *IoSyslog_rawClone(IoSyslog *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoSyslogData)));
	return self;
}

IoSyslog *IoSyslog_new(void *state)
{
	IoSyslog *proto = IoState_protoWithInitFunction_(state, IoSyslog_proto);
	return IOCLONE(proto);
}

void IoSyslog_free(IoSyslog *self)
{
	free(IoObject_dataPointer(self));
}

/* ----------------------------------------------------------- */

void syslog_write(int pri, char *msg)
{
	syslog(pri, "%s", msg);
}

IoObject *IoSyslog_open(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog open(aPriority, someOptions, optionalIdentity)
	Opens the syslog for writing. optionalIdentity need not be entered 
	and will default to the name of the distribution of Io you are running 
	or if you have embedded Io into your application and set 
	Lobby distribution = "foo", it will be set to "foo".
	*/
	 
	int syslog_facility, syslog_options;
	//int i, max;
	char *syslog_ident;

	if (DATA(self)->syslog_opened)
	{
		IoState_error_(IOSTATE, m, "System log is already open");
		return IONIL(self);
	}

	{
		DATA(self)->facility = IOREF(IoMessage_locals_numberArgAt_(m, locals, 0));
		if (ISNIL(DATA(self)->facility))
		{
			syslog_facility = LOG_USER;
		}
		else
		{
			syslog_facility = IoObject_dataUint32(DATA(self)->facility);
		}

		DATA(self)->options = IOREF(IoMessage_locals_listArgAt_(m, locals, 1));
		syslog_options = 0;
		if (ISNIL(DATA(self)->options))
		{
			syslog_options = LOG_PID | LOG_CONS;
		}
		else
		{
			List *list = IoList_rawList(DATA(self)->options);

			LIST_FOREACH(list, i, v,
				syslog_options |= (int)CNUMBER(v);
			);
		}

		syslog_ident = (char *)IOSYMBOL_BYTES(DATA(self)->ident);
		if ((strlen(syslog_ident) == 0) || ISNIL(DATA(self)->ident))
		{
			char *s = CSTRING(IoState_doCString_(IOSTATE, "Lobby distribution"));
			strncpy(syslog_ident, s, strlen(s));
		}

		openlog(syslog_ident, syslog_options, syslog_facility);
		DATA(self)->syslog_opened = 1;
		DATA(self)->syslog_mask = setlogmask(0);
		setlogmask(DATA(self)->syslog_mask);
	}

	return self;
}

IoObject *IoSyslog_reopen(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog reopen(aFacility, someOptions, optionalIdentity) 
	Reopens an already open log session. This is useful if you wish to 
	change the facility you are logging to, the options you are logging 
	with, or the identity of the session. Takes the same options as the open slot.
	*/
	
	/* If the log is already opened, close it, if not, no big deal. */
	if (DATA(self)->syslog_opened)
	{
		closelog();
		DATA(self)->syslog_opened = 0;
	}
	IoSyslog_open(self, locals, m);
	return self;
}

IoObject *IoSyslog_isOpen(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog isOpen
	Returns self if the log is opened for writing. Otherwise, returns Nil.")
	*/
	
	return IOBOOL(self, DATA(self)->syslog_opened);
}

IoObject *IoSyslog_close(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog close
	Closes a log that has previously been opened for writing.")
	*/
	
	if (!DATA(self)->syslog_opened)
	{
		IoState_error_(IOSTATE, m, "Log is not open");
		return IONIL(self);
	}

	closelog();
	DATA(self)->syslog_opened = 0;

	return self;
}

IoObject *IoSyslog_identity(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog identity(optionalIdentity)
	If optionalIdentity is specified, provides an identity for all of the messages you will be sending to the syslog daemon. Returns the identity.")
	*/
	
	if (IoMessage_argCount(m) >= 1)
	{
		DATA(self)->ident = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	}
	return DATA(self)->ident;
}

IoObject *IoSyslog_options(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog options(optionalOptions)
	If optionalOptions is specified, it should represent a list of the logging 
	options you can find in the optionsMap slot. All the values in the supplied 
	aList will be OR'd together when you call the open or reopen slots. 
	Returns the list of options if optionalFacility is omitted.
	*/
	
	if (IoMessage_argCount(m) >= 1)
	{
		DATA(self)->options = IOREF(IoMessage_locals_listArgAt_(m, locals, 0));
	}
	return DATA(self)->options;
}

IoObject *IoSyslog_optionsMap(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog optionsMap
	A map containing key/value pairs holding all available options. These include:
	<p>
	<ul>
	<li>LOG_PID</li>
	<li>LOG_CONS</li>
	<li>LOG_ODELAY</li>
	<li>LOG_NDELAY</li>
	<li>LOG_NOWAIT</li>
	<li>LOG_PERROR</li>
	</ul>
	*/
	
	PHash *map = IoObject_dataPointer(DATA(self)->optionsMap);

	PHash_at_put_(map, IOSYMBOL("LOG_PID"), IONUMBER(1));
	PHash_at_put_(map, IOSYMBOL("LOG_CONS"), IONUMBER(2));
	PHash_at_put_(map, IOSYMBOL("LOG_ODELAY"), IONUMBER(4));
	PHash_at_put_(map, IOSYMBOL("LOG_NDELAY"), IONUMBER(8));
	PHash_at_put_(map, IOSYMBOL("LOG_NOWAIT"), IONUMBER(16));
	PHash_at_put_(map, IOSYMBOL("LOG_PERROR"), IONUMBER(32));

	return DATA(self)->optionsMap;
}

IoObject *IoSyslog_facility(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog facility(optionalFacility)
	Specifies the logging facility, which can be one of any of the values 
	found in the facilityMap map. If optionalFacility is omitted, returns 
	the currently set facility.
	*/
	if (IoMessage_argCount(m) >= 1)
	{
		DATA(self)->facility = IOREF(IoMessage_locals_numberArgAt_(m, locals, 0));
	}
	return DATA(self)->facility;
}

IoObject *IoSyslog_facilityMap(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog facilityMap
	Contains the following keys, which represent numbers that can be used when opening a log:
	<p>
	<ul>
	 <li>LOG_KERN</li>
	 <li>LOG_USER</li>
	 <li>LOG_MAIL</li>
	 <li>LOG_DAEMON</li>
	 <li>LOG_AUTH</li>
	 <li>LOG_SYSLOG</li>
	 <li>LOG_LPR</li>
	 <li>LOG_NEWS</li>
	 <li>LOG_UUCP</li>
	 <li>LOG_CRON</li>
	 <li>LOG_AUTHPRIV</li>
	 <li>LOG_FTP</li>
	 <li>LOG_RESERVED0</li>
	 <li>LOG_RESERVED1</li>
	 <li>LOG_RESERVED2</li>
	 <li>LOG_RESERVED3</li>
	 <li>LOG_LOCAL0</li>
	 <li>LOG_LOCAL1</li>
	 <li>LOG_LOCAL2</li>
	 <li>LOG_LOCAL3</li>
	 <li>LOG_LOCAL4</li>
	 <li>LOG_LOCAL5</li>
	 <li>LOG_LOCAL6</li>
	 <li>LOG_LOCAL7</li>
	</ul>
	 */
	 
	PHash *map = IoObject_dataPointer(DATA(self)->facilityMap);

	PHash_at_put_(map, IOSYMBOL("LOG_KERN"), IONUMBER(0));
	PHash_at_put_(map, IOSYMBOL("LOG_USER"), IONUMBER(8));
	PHash_at_put_(map, IOSYMBOL("LOG_MAIL"), IONUMBER(16));
	PHash_at_put_(map, IOSYMBOL("LOG_DAEMON"), IONUMBER(24));
	PHash_at_put_(map, IOSYMBOL("LOG_AUTH"), IONUMBER(32));
	PHash_at_put_(map, IOSYMBOL("LOG_SYSLOG"), IONUMBER(40));
	PHash_at_put_(map, IOSYMBOL("LOG_LPR"), IONUMBER(48));
	PHash_at_put_(map, IOSYMBOL("LOG_NEWS"), IONUMBER(56));
	PHash_at_put_(map, IOSYMBOL("LOG_UUCP"), IONUMBER(64));
	PHash_at_put_(map, IOSYMBOL("LOG_CRON"), IONUMBER(72));
	PHash_at_put_(map, IOSYMBOL("LOG_AUTHPRIV"), IONUMBER(80));
	PHash_at_put_(map, IOSYMBOL("LOG_FTP"), IONUMBER(88));
	PHash_at_put_(map, IOSYMBOL("LOG_RESERVED0"), IONUMBER(96));
	PHash_at_put_(map, IOSYMBOL("LOG_RESERVED1"), IONUMBER(104));
	PHash_at_put_(map, IOSYMBOL("LOG_RESERVED2"), IONUMBER(112));
	PHash_at_put_(map, IOSYMBOL("LOG_RESERVED3"), IONUMBER(120));
	PHash_at_put_(map, IOSYMBOL("LOG_LOCAL0"), IONUMBER(128));
	PHash_at_put_(map, IOSYMBOL("LOG_LOCAL1"), IONUMBER(136));
	PHash_at_put_(map, IOSYMBOL("LOG_LOCAL2"), IONUMBER(144));
	PHash_at_put_(map, IOSYMBOL("LOG_LOCAL3"), IONUMBER(152));
	PHash_at_put_(map, IOSYMBOL("LOG_LOCAL4"), IONUMBER(160));
	PHash_at_put_(map, IOSYMBOL("LOG_LOCAL5"), IONUMBER(168));
	PHash_at_put_(map, IOSYMBOL("LOG_LOCAL6"), IONUMBER(176));
	PHash_at_put_(map, IOSYMBOL("LOG_LOCAL7"), IONUMBER(184));

	return DATA(self)->facilityMap;
}

IoObject *IoSyslog_priority(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog priority(optionalPriority)
	If optionalPriority is specified, sets the value, and returns it. 
	If no value is specified, will return the previously stored value if 
	one has been set previously.
	*/

	if (IoMessage_argCount(m) >= 1)
	{
		DATA(self)->priority = IOREF(IoMessage_locals_numberArgAt_(m, locals, 0));
	}

	return DATA(self)->priority;
}

IoObject *IoSyslog_priorityMap(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog  priorityMap
	Contains key/value pairs for logging priorities for use when calling the log() method. These include:
	<p>
	<ul>
	<li>LOG_EMERG</li>
	<li>LOG_ALERT</li>
	<li>LOG_CRIT</li>
	<li>LOG_ERR</li>
	<li>LOG_WARNING</li>
	<li>LOG_NOTICE</li>
	<li>LOG_INFO</li>
	<li>LOG_DEBUG</li>
	</ul>
	*/
	 
	PHash *map = IoObject_dataPointer(DATA(self)->priorityMap);
	PHash_at_put_(map, IOSYMBOL("LOG_EMERG"), IONUMBER(0));
	PHash_at_put_(map, IOSYMBOL("LOG_ALERT"), IONUMBER(1));
	PHash_at_put_(map, IOSYMBOL("LOG_CRIT"), IONUMBER(2));
	PHash_at_put_(map, IOSYMBOL("LOG_ERR"), IONUMBER(3));
	PHash_at_put_(map, IOSYMBOL("LOG_WARNING"), IONUMBER(4));
	PHash_at_put_(map, IOSYMBOL("LOG_NOTICE"), IONUMBER(5));
	PHash_at_put_(map, IOSYMBOL("LOG_INFO"), IONUMBER(6));
	PHash_at_put_(map, IOSYMBOL("LOG_DEBUG"), IONUMBER(7));

	return DATA(self)->priorityMap;
}

IoObject *IoSyslog_mask(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog mask(optionalMask)
	If optionalMask is specified, optionalMask is a list which contains 
	any one or more values stored in the maskMap hash that will be OR'd 
	together, to provide the proper mask. Returns the logging mask (as a List).
	*/
	 
	if (IoMessage_argCount(m) >= 1)
	{
		if (!DATA(self)->syslog_opened)
		{
			IoState_error_(IOSTATE, m, "Log must be opened before setting the logging mask");
			return IONIL(self);
		}

		DATA(self)->mask = IOREF(IoMessage_locals_listArgAt_(m, locals, 0));

		{
			List *list = IoList_rawList(DATA(self)->mask);

			LIST_FOREACH(list, i, v,
				DATA(self)->syslog_mask |= (int)CNUMBER(v);
			);

			setlogmask(DATA(self)->syslog_mask);
		}
	}

	return DATA(self)->mask;
}

IoObject *IoSyslog_maskMap(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog maskMap
	Contains keys/value pairs which represent numbers that specify the 
	logging mask. These values may be any one (or more) of the following:
	<p>
	<ul>
	<li>LOG_PRIMASK</li>
	<li>LOG_FACMASK</li>
	</ul>
	*/
	 
	PHash *map = IoObject_dataPointer(DATA(self)->maskMap);

	PHash_at_put_(map, IOSYMBOL("LOG_PRIMASK"), IONUMBER(0x07));
	PHash_at_put_(map, IOSYMBOL("LOG_FACMASK"), IONUMBER(0x03f8));

	return DATA(self)->maskMap;
}

IoObject *IoSyslog_log(IoSyslog *self, IoObject *locals, IoMessage *m)
{
	/*doc Syslog log
	Writes the supplied data to the log. Requires 2 arguments:
	<p>
	<ul>
	<li>Logging Priority</li>
	<li>Message to log</li>
	</ul>
	*/
	 
	char *str;

	DATA(self)->priority = IOREF(IoMessage_locals_numberArgAt_(m, locals, 0));
	str = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 1));

	if (!DATA(self)->syslog_opened)
	{
		IoState_error_(IOSTATE, m, "Log is not opened. Run the open slot before log.");
		return IONIL(self);
	}

	syslog_write(CNUMBER((uintptr_t)DATA(self)->priority), str);

	return self;
}
