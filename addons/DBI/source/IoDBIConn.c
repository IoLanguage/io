//metadoc DBIConn copyright Jeremy Cowgar 2006
//metadoc DBIConn license BSD revised
//metadoc DBIConn category Databases
//metadoc DBIConn description An object that represents a DBI Connection.

#include <dbi/dbi.h>

#include "IoDBI.h"
#include "IoDBIResult.h"
#include "IoDBIConn.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoMap.h"

#define DATA(self) ((IoDBIConnData *)IoObject_dataPointer(self))

static const char *protoId = "DBIConn";

IoTag *IoDBIConn_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoDBIConn_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoDBIConn_free);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoDBIConn_mark);
	return tag;
}

IoDBIConn *IoDBIConn_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoDBIConn_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoDBIConnData)));
	DATA(self)->conn = NULL;

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
			{"driver", IoDBIConn_driver},
			{"optionPut", IoDBIConn_optionPut},
			{"option", IoDBIConn_option},
			{"optionClear", IoDBIConn_optionClear},
			{"options", IoDBIConn_options},
			{"optionsClear", IoDBIConn_optionsClear},
			{"connect", IoDBIConn_connect},
			{"close", IoDBIConn_close},
			{"ping", IoDBIConn_ping},
			{"quote", IoDBIConn_quote},
			{"query", IoDBIConn_query},
			{"execute", IoDBIConn_execute},
			{"sequenceLast", IoDBIConn_lastSequence},
			{"sequenceNext", IoDBIConn_nextSequence},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoDBIConn *IoDBIConn_rawClone(IoDBIConn *proto)
{
	IoDBIConn *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoDBIConnData)));
	DATA(self)->conn = NULL;
	return self;
}

void IoDBIConn_free(IoDBIConn *self)
{
	if (DATA(self)->conn)
	{
		dbi_conn_close(DATA(self)->conn);
		DATA(self)->conn = NULL;
	}

	free(DATA(self));
}

//void IoDBIConn_mark(IoDBIConn *self) {}

/* ---------------------------------------------------------------- */

IoDBIConn *IoDBIConn_new(void *state, dbi_conn conn)
{
	IoDBIConn *self = IOCLONE(IoState_protoWithId_(state, protoId));
	DATA(self)->conn = conn;
	return self;
}

/* ---------------------------------------------------------------- */

IoObject *IoDBIConn_driver(IoDBIConn *self, IoObject *locals, IoMessage *m)
{
	//doc DBIConn driver Return database driver name.
	
	dbi_driver d = dbi_conn_get_driver(DATA(self)->conn);
	return IOSYMBOL(dbi_driver_get_name(d));
}

IoObject *IoDBIConn_optionPut(IoDBIConn *self, IoObject *locals, IoMessage *m)
{
	//doc DBIConn optionPut(key,value) Add an option key, value pair to the connection.

	IoObject *key = IoMessage_locals_valueArgAt_(m, locals, 0);
	IoObject *val = IoMessage_locals_valueArgAt_(m, locals, 1);

	if (!ISSYMBOL(key))
	{
		IoState_error_(IOSTATE, m,
				"argument 0 to method '%s' must be a Symbol, not a '%s'\n",
				CSTRING(IoMessage_name(m)), IoObject_name(key));
		return IOBOOL(self, 0);
	}

	if (!ISSYMBOL(val))
	{
		IoState_error_(IOSTATE, m,
				"argument 0 to method '%s' must be a Symbol, not a '%s'\n",
				CSTRING(IoMessage_name(m)), IoObject_name(key));
		return IOBOOL(self, 0);
	}

	if (0 != dbi_conn_set_option(DATA(self)->conn, CSTRING(key), CSTRING(val)))
	{
		ReportDBIError(DATA(self)->conn, IOSTATE, m);
	}

	return IOBOOL(self, 1);
}

IoObject *IoDBIConn_option(IoDBIConn *self, IoObject *locals, IoMessage *m)
{
	//doc DBIConn option(key) Retrieve an option value for the connection.
	
	IoObject *key = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (!ISSYMBOL(key))
	{
		IoState_error_(IOSTATE, m,
				"argument 0 to method '%s' must be a Symbol, not a '%s'\n",
				CSTRING(IoMessage_name(m)), IoObject_name(key));
		return IONIL(self);
	}

	return IOSYMBOL(dbi_conn_get_option(DATA(self)->conn, CSTRING(key)));
}

IoObject *IoDBIConn_options(IoDBIConn *self, IoObject *locals,
			IoMessage *m)
{
	//doc DBIConn options Retrieve an option list of the connection.
	
	IoList *list = IOREF(IoList_new(IOSTATE));
	const char *option = NULL;

	while((option = dbi_conn_get_option_list(DATA(self)->conn, option)) != NULL)
	{
		IoList_rawAppend_(list, IOSYMBOL(option));
	}

	return list;
}

IoObject *IoDBIConn_optionsClear(IoDBIConn *self, IoObject *locals,
			IoMessage *m)
{
	//doc DBIConn optionsClear Clear all options associated with the connection
	
	dbi_conn_clear_options(DATA(self)->conn);
	return IONIL(self);
}

IoObject *IoDBIConn_optionClear(IoDBIConn *self, IoObject *locals,
			IoMessage *m)
{
	//doc DBIConn optionClear(key) Clear a specific option associated with the connection.

	IoObject *key = IoMessage_locals_valueArgAt_(m, locals, 0);
	
	if (!ISSYMBOL(key))
	{
		IoState_error_(IOSTATE, m,
				"argument 0 to method '%s' must be a Symbol, not a '%s'\n",
				CSTRING(IoMessage_name(m)), IoObject_name(key));
	}

	dbi_conn_clear_option(DATA(self)->conn, CSTRING(key));

	return IONIL(self);
}

IoObject *IoDBIConn_connect(IoDBIConn *self, IoObject *locals, IoMessage *m)
{
	//doc DBIConn connect Connect to the database server.
	
	if (0 != dbi_conn_connect(DATA(self)->conn))
	{
		ReportDBIError(DATA(self)->conn, IOSTATE, m);
	}

	return IOBOOL(self, 1);
}

IoObject *IoDBIConn_close(IoDBIConn *self, IoObject *locals, IoMessage *m)
{
	//doc DBIConn close Close the connection to the database.
	
	if (DATA(self)->conn) dbi_conn_close(DATA(self)->conn);
	DATA(self)->conn = NULL;
	return IONIL(self);
}

IoObject *IoDBIConn_ping(IoDBIConn *self, IoObject *locals, IoMessage *m)
{
	/*doc DBIConn ping 
	Ping the database to see if it's alive. 
	Will return true if it is, otherwise false.
	*/
	
	return IOBOOL(self, dbi_conn_ping(DATA(self)->conn) == 1);
}

IoObject *IoDBIConn_quote(IoDBIConn *self, IoObject *locals, IoMessage *m)
{
	/*doc DBIConn quote(value)
	Quote a string value according to the database server's specifications.
	*/
	
	char *value = NULL, *v2;
	size_t newLen;
	IoObject *ret;

	IoObject *valueArg = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (!ISSYMBOL(valueArg))
	{
		IoState_error_(IOSTATE, m,
				"argument 0 to method '%s' must be a Symbol, not a '%s'\n",
				CSTRING(IoMessage_name(m)), IoObject_name(valueArg));
	}

	value = CSTRING(valueArg);

	v2 = malloc(strlen(value) + 1);
	strcpy(v2, value);

	newLen = dbi_conn_quote_string(DATA(self)->conn, &v2);

	if (0 == newLen)
	{
		ReportDBIError(DATA(self)->conn, IOSTATE, m);
	}

	ret = IOSYMBOL(v2);

	free(v2);

	return ret;
}

IoObject *IoDBIConn_query(IoDBIConn *self, IoObject *locals, IoMessage *m)
{
	/*doc DBIConn query(sql_query)
	Perform a database query returning a DBIResult object.
	*/
	
	dbi_result result;

	IoObject *sql = IoMessage_locals_valueArgAt_(m, locals, 0);
	
	if (!ISSYMBOL(sql))
	{
		IoState_error_(IOSTATE, m,
				"argument 0 to method '%s' must be a Symbol, not a '%s'\n",
				CSTRING(IoMessage_name(m)), IoObject_name(sql));
	}

	result = dbi_conn_query(DATA(self)->conn, CSTRING(sql));
	
	if (result == NULL)
	{
		const char *error;
		
		int errorCode = dbi_conn_error(DATA(self)->conn, &error);

		IoState_error_(IOSTATE, m, "Could not perform query '%s' "
					"libdbi: %i: %s\n", CSTRING(sql), errorCode, error);
	}

	return IoDBIResult_new(IOSTATE, result);
}

IoObject *IoDBIConn_execute(IoDBIConn *self, IoObject *locals, IoMessage *m)
{
	/*doc DBIConn execute(sql_query)
	Perform a database query that expects no results. 
	Returns the number of rows affected.
	*/
	
	dbi_result result;
	unsigned long long affectedRows = 0;

	IoObject *sql = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (!ISSYMBOL(sql))
	{
		IoState_error_(IOSTATE, m,
				"argument 0 to method '%s' must be a Symbol, not a '%s'\n",
				CSTRING(IoMessage_name(m)), IoObject_name(sql));
	}

	result = dbi_conn_query(DATA(self)->conn, CSTRING(sql));
	if (result == NULL)
	{
		const char *error;
		int errorCode = dbi_conn_error(DATA(self)->conn, &error);

		IoState_error_(IOSTATE, m, "Could not perform query '%s' "
					"libdbi: %i: %s\n", CSTRING(sql), errorCode, error);
	}

	affectedRows = dbi_result_get_numrows_affected(result);

	dbi_result_free(result);

	return IONUMBER(affectedRows);
}

IoObject *IoDBIConn_lastSequence(IoDBIConn *self, IoObject *locals,
			IoMessage *m)
{
	/*doc DBIConn sequenceLast([name])
	Return the last inserted sequence value.
	*/
	
	char *name = NULL;
	
	if (IoMessage_argCount(m) == 1)
	{
		IoObject *nameArg = IoMessage_locals_valueArgAt_(m, locals, 0);

		if (!ISSYMBOL(nameArg))
		{
			IoState_error_(IOSTATE, m,
					"argument 0 to method '%s', if supplied, must be a Symbol, "
					"not a '%s'", CSTRING(IoMessage_name(m)),
					IoObject_name(nameArg));
		}

		name = CSTRING(nameArg);
	}

	return IONUMBER(dbi_conn_sequence_last(DATA(self)->conn, name));
}

IoObject *IoDBIConn_nextSequence(IoDBIConn *self, IoObject *locals,
			IoMessage *m)
{
	/*doc DBIConn sequenceNext([name])
	Return the next sequence that will be used during an INSERT query.
	*/
	
	char *name = NULL;
	
	if (IoMessage_argCount(m) == 1)
	{
		IoObject *nameArg = IoMessage_locals_valueArgAt_(m, locals, 0);

		if (!ISSYMBOL(nameArg))
		{
			IoState_error_(IOSTATE, m,
					"argument 0 to method '%s', if supplied, must be a Symbol, "
					"not a '%s'", CSTRING(IoMessage_name(m)),
					IoObject_name(nameArg));
		}

		name = CSTRING(nameArg);
	}

	return IONUMBER(dbi_conn_sequence_next(DATA(self)->conn, name));
}
