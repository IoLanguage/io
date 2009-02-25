//metadoc DBIResult copyright Jeremy Cowgar, 2006
//metadoc DBIResult license BSD revised
//metadoc DBIResult category Databases
//metadoc DBIResult description A DBI Result created by a call to DBIConn query.

#include "IoMessage.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoDate.h"
#include "IoMap.h"

#include "IoDBI.h"
#include "IoDBIResult.h"
#include "IoDBIConn.h"

#define DATA(self) ((IoDBIResultData *)IoObject_dataPointer(self))

IoTag *IoDBIResult_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("DBIResult");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoDBIResult_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoDBIResult_free);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoDBIResult_mark);
	return tag;
}

IoDBIResult *IoDBIResult_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoDBIResult_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoDBIResultData)));
	DATA(self)->conn = NULL;
	DATA(self)->result = NULL;

	IoState_registerProtoWithFunc_(state, self, IoDBIResult_proto);

	{
		IoMethodTable methodTable[] = {
			{"first", IoDBIResult_first},
			{"previous", IoDBIResult_previous},
			{"next", IoDBIResult_next},
			{"last", IoDBIResult_last},
			{"seek", IoDBIResult_seek},
			{"position", IoDBIResult_position},
			{"foreach", IoDBIResult_foreach},
			{"at", IoDBIResult_at},
			{"populate", IoDBIResult_populate},
			{"size", IoDBIResult_size},
			{"fields", IoDBIResult_fields},
			{"done", IoDBIResult_done},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoDBIResult *IoDBIResult_rawClone(IoDBIResult *proto)
{
	IoDBIResult *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoDBIResultData)));
	DATA(self)->conn = NULL;
	DATA(self)->result = NULL;
	return self;
}

void IoDBIResult_free(IoDBIResult *self)
{
	free(IoObject_dataPointer(self));
}

//void IoDBIResult_mark(IoDBIResult *self) {}

/* ---------------------------------------------------------------- */

IoDBIResult *IoDBIResult_new(void *state, dbi_result result)
{
	IoDBIResult *self = IOCLONE(IoState_protoWithInitFunction_(state,
				IoDBIResult_proto));

	DATA(self)->result = result;
	DATA(self)->conn = dbi_result_get_conn(result);

	return self;
}

IoObject *IoDBIResult_getIoObjectFromResult_(IoObject *self, dbi_result res,
			int index)
{
	const char *val = NULL;

	switch (dbi_result_get_field_type_idx(res, index))
	{
		case DBI_TYPE_INTEGER:
			return IONUMBER(dbi_result_get_int_idx(res, index));
		case DBI_TYPE_DECIMAL:
			return IONUMBER(dbi_result_get_double_idx(res, index));
		case DBI_TYPE_STRING:
			val = dbi_result_get_string_idx(res, index);
			/* Some DB's do not report is_null even when the are, :-( */
			if (1 == dbi_result_field_is_null_idx(res, index) || val == NULL)
				return IONIL(self);
			else
				return IOSYMBOL(dbi_result_get_string_idx(res, index));
		case DBI_TYPE_BINARY:
			return IOSYMBOL(dbi_result_get_binary_idx(res, index));
		case DBI_TYPE_DATETIME:
			return IoDate_newWithTime_(IOSTATE, dbi_result_get_datetime_idx(
					res, index));
	}

	return IONIL(self);
}

IoObject *IoDBIResult_rowToMap_(void *state, IoDBIResult *self,
			dbi_result res)
{
	int fIdx, fCount = dbi_result_get_numfields(res);
	IoMap *map = IoMap_new(state);

	for (fIdx = 1; fIdx <= fCount; fIdx++)
	{
		IoMap_rawAtPut(map,
				IOSYMBOL(dbi_result_get_field_name(res, fIdx)),
				IoDBIResult_getIoObjectFromResult_(self, res, fIdx));
	}

	return map;
}

/* ---------------------------------------------------------------- */

IoObject *IoDBIResult_size(IoDBIResult *self, IoObject *locals,
			IoMessage *m)
{
	//doc DBIResult size Returns the number of rows available.

	return IONUMBER(dbi_result_get_numrows(DATA(self)->result));
}

IoObject *IoDBIResult_fields(IoDBIResult *self, IoObject *locals,
			IoMessage *m)
{
	//doc DBIResult fields Returns a list of field names in the result.

	int idx;
	IoList *list = IOREF(IoList_new(IOSTATE));

	for (idx = 1; idx <= dbi_result_get_numfields(DATA(self)->result); idx++)
	{
		IoList_rawAppend_(list, IOSYMBOL(dbi_result_get_field_name(
				DATA(self)->result, idx)));
	}

	return list;
}

IoObject *IoDBIResult_first(IoDBIResult *self, IoObject *locals,
			IoMessage *m)
{
	//doc DBIResult first Move the cursor to the first record.
	
	if (1 != dbi_result_first_row(DATA(self)->result))
	{
		ReportDBIError(DATA(self)->conn, IOSTATE, m);
	}

	return IOBOOL(self, 1);
}

IoObject *IoDBIResult_previous(IoDBIResult *self, IoObject *locals,
			IoMessage *m)
{
	//doc DBIResult previous Move the cursor to the previous record.
	
	if (1 != dbi_result_prev_row(DATA(self)->result))
	{
		ReportDBIError(DATA(self)->conn, IOSTATE, m);
	}

	return IOBOOL(self, 1);
}

IoObject *IoDBIResult_next(IoDBIResult *self, IoObject *locals,
			IoMessage *m)
{
	//doc DBIResult next Move the cursor to the next record.
	
	dbi_result res = DATA(self)->result;

	if (0 == dbi_result_next_row(res))
	{
		ReportDBIError(DATA(self)->conn, IOSTATE, m);
	}

	return IOBOOL(self,
				dbi_result_get_currow(res) < dbi_result_get_numrows(res));
}

IoObject *IoDBIResult_last(IoDBIResult *self, IoObject *locals,
			IoMessage *m)
{
	//doc DBIResult last Move the cursor to the last record.
	
	dbi_result res = DATA(self)->result;

	if (0 == dbi_result_last_row(res))
	{
		ReportDBIError(DATA(self)->conn, IOSTATE, m);
	}

	return IOBOOL(self, 1);
}


IoObject *IoDBIResult_seek(IoDBIResult *self, IoObject *locals,
			IoMessage *m)
{
	//doc DBIResult seek(row_number) Move the cursor to the nth record.

	long rowIdx;
	dbi_result res = DATA(self)->result;
	IoObject *row = IoMessage_locals_valueArgAt_(m, locals, 0);

	if (!ISNUMBER(row))
	{
		IoState_error_(IOSTATE, m,
				"argument 0 to method '%s' must be a Number, not a '%s'\n",
				CSTRING(IoMessage_name(m)), IoObject_name(row));
	}

	rowIdx = IoNumber_asLong(row);
	
	if (1 != dbi_result_seek_row(res, rowIdx))
	{
		const char *error;
		
		int errorCode = dbi_conn_error(DATA(self)->conn, &error);

		if (errorCode == 0)
		{
			IoState_error_(IOSTATE, m, "row index %i out of range (1,%i)\n",
					rowIdx, dbi_result_get_numrows(res));
		}
		else
		{
			IoState_error_(IOSTATE, m, "libdbi: %i: %s\n", errorCode, error);
		}
	}

	return IOBOOL(self, 1);
}

IoObject *IoDBIResult_position(IoDBIResult *self, IoObject *locals, IoMessage *m)
{
	//doc DBIResult position Return the current row's position (or index).

	unsigned long long rowNum = dbi_result_get_currow(DATA(self)->result);

	if (0 == rowNum)
	{
		ReportDBIError(DATA(self)->conn, IOSTATE, m);
	}

	return IONUMBER(rowNum);
}

IoObject *IoDBIResult_at(IoDBIResult *self, IoObject *locals, IoMessage *m)
{
	/*doc DBIResult at(index_or_name)
	 Returns the contents of the given field. 
	 The parameter can be a field index or a field name.
	*/
	unsigned int idx = 0;
	dbi_result res = DATA(self)->result;

	IoObject *key = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (ISNUMBER(key))
	{
		idx = IoNumber_asInt(key);
	}
	else if (ISSYMBOL(key))
	{
		idx = dbi_result_get_field_idx(res, CSTRING(key));
		if (0 == idx)
		{
			ReportDBIError(DATA(self)->conn, IOSTATE, m);
		}
	}
	else
	{
		IoState_error_(IOSTATE, m,
				"argument 0 to method '%s' must be a Number or Symbol, not a '%s'\n",
				CSTRING(IoMessage_name(m)), IoObject_name(key));
	}

	return IoDBIResult_getIoObjectFromResult_(self, res, idx);
}

IoObject *IoDBIResult_populate(IoDBIResult *self, IoObject *locals,
			IoMessage *m)
{
	/*doc DBIResult populate(object)
	Populates a decendent of DBIRecord with the current record's contents. 
	See `DBIRecord' for further explanation and an example.
	*/
	
	dbi_result res = DATA(self)->result;
	IoObject *baseObject = IoMessage_locals_valueArgAt_(m, locals, 0);
	IoObject *o = IOCLONE(baseObject);
	o = IoObject_initClone_(self, locals, m, o);

	IoObject_setSlot_to_(o, IOSYMBOL("_map"),
			IoDBIResult_rowToMap_(IOSTATE, self, res));

	return o;
}

IoObject *IoDBIResult_foreach(IoDBIResult *self, IoObject *locals,
			IoMessage *m)
{
/*doc DBIResult foreach([Object], value, message)
Loops over the records in the result starting at either the first result 
(if the cursor has never been moved) or it's current location if moved. i.e.

<pre>
r := conn query("SELECT * FROM people")
r foreach(r, r at(1))
</pre>

The above would start at the first row, however, you can move around in the
result set and then foreach would pickup where you left off, for instance, say
you wanted to skip the first three rows:

<pre>
r := conn query("SELECT * FROM people")
r seek(4)
r foreach(r, r at (1))
</pre>

The above would start at the record #4, not at the beginning.

The optional Object parameter would cause a decendent of DBIRecord to be
populate instead of the index being set. This allows for advanced
functionality. Please see `DBIRecord' for further information and an example.
*/
	
	dbi_result res = DATA(self)->result;
	IoObject *result = IONIL(self);
	IoMessage *doMessage;
	IoSymbol *baseObject = NULL;
	IoSymbol *resSlotName;

	unsigned int i, count = dbi_result_get_numrows(res);
	unsigned int fIdx, fCount = dbi_result_get_numfields(res);

	/* If there are 0 resulting rows, we don't have to loop */
	if(count == 0) {
		return result;
	}

	if (IoMessage_argCount(m) == 2)
	{
		resSlotName = IoMessage_name(IoMessage_rawArgAt_(m, 0));
		doMessage = IoMessage_rawArgAt_(m, 1);
	}
	else if (IoMessage_argCount(m) == 3)
	{
		baseObject = IoMessage_locals_valueArgAt_(m, locals, 0);
		resSlotName = IoMessage_name(IoMessage_rawArgAt_(m, 1));
		doMessage = IoMessage_rawArgAt_(m, 2);
	}
	else
	{
		IoState_error_(IOSTATE, m,
				"method '%s' takes 2 or 3 parameters, you supplied %i\n",
				IoMessage_argCount(m));
	}

	IoState_pushRetainPool(IOSTATE);

	if (0 == dbi_result_get_currow(res))
	{
		if (0 == dbi_result_first_row(res))
		{
			ReportDBIError(DATA(self)->conn, IOSTATE, m);
		}
	}

	for (i = dbi_result_get_currow(res); i <= count; i++)
	{
		IoState_clearTopPool(IOSTATE);

		if (i == 0)
		{
			ReportDBIError(DATA(self)->conn, IOSTATE, m);
		}

		if (baseObject)
		{
			IoObject *o = IOCLONE(baseObject);
			o = IoObject_initClone_(self, locals, m, o);

			IoObject_setSlot_to_(o, IOSYMBOL("_map"),
					IoDBIResult_rowToMap_(IOSTATE, self, res));
			IoObject_setSlot_to_(locals, resSlotName, o);
		}
		else {
			IoObject_setSlot_to_(locals, resSlotName, self);
		}

		result = IoMessage_locals_performOn_(doMessage, locals, locals);

		if (IoState_handleStatus(IOSTATE))
		{
			goto done;
		}

		if (i != count && 0 == dbi_result_next_row(DATA(self)->result))
		{
			ReportDBIError(DATA(self)->conn, IOSTATE, m);
		}
	}

done:
	IoState_popRetainPoolExceptFor_(IOSTATE, result);

	return result;
}

IoObject *IoDBIResult_done(IoDBIResult *self, IoObject *locals, IoMessage *m)
{
	/*doc DBIResult done
	Close and free the result. This <b>must</b> be called on
each result. Failure to do so will cause memory leaks and open queries with
the database server.
	*/
	
	if (0 != dbi_result_free(DATA(self)->result))
	{
		ReportDBIError(DATA(self)->conn, IOSTATE, m);
	}

	DATA(self)->result = NULL;

	return IONIL(self);
}
