/*metadoc SQLite copyright 
	Steve Dekorte, 2004
*/

/*metadoc SQLite license 
	BSD revised
*/

/*metadoc SQLite category
	Databases
*/

/*metadoc SQLite description 
SQLite provides a embedded simple and fast (2x faster than PostgreSQL or MySQL) SQL database. See http://www.hwaci.com/sw/sqlite/ for details. It's SQL command set is described at http://www.hwaci.com/sw/sqlite/lang.html. SQLite was written by Dr. Richard Hipp who offers consulting services for custom modifications and support of SQLite. Example:
<pre>	
db := SQLite clone
db setPath("myDatabase.sqlite")
db open
db exec("CREATE TABLE Dbm (key, value)")
db exec("CREATE INDEX DbmIndex ON Dbm (key)")
db exec("INSERT INTO Dbm ('key', 'value') VALUES ('a', '123')")
db exec("INSERT INTO Dbm ('key', 'value') VALUES ('a', 'efg')")
rows := db exec("SELECT key, value FROM Dbm WHERE key='a'")
db exec("DELETE FROM Dbm WHERE key='a'")
rows := db exec("SELECT key, value FROM Dbm WHERE key='a'")
db close
</pre>
*/

#include "IoSQLite.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoMap.h"

typedef int (ResultRowCallback)(void *, int , char **, char **);

#define DATA(self) ((IoSQLiteData *)IoObject_dataPointer(self))
static const char *protoId = "SQLite";

/*
 static int IoSQLite_resultRow(void *context, int argc, char **argv, char **azColName);
 static int IoSQLite_busyHandler(void *context, const char *s, int n);
 */

IoTag *IoSQLite_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSQLite_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSQLite_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSQLite_mark);
	return tag;
}

IoSQLite *IoSQLite_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSQLite_newTag(state));

	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoSQLiteData)));
	DATA(self)->path = IOSYMBOL(".");
	IoSQLite_error_(self, "");

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"setPath", IoSQLite_setPath},
		{"path", IoSQLite_path},
		{"open", IoSQLite_open},
		{"close", IoSQLite_close},
		{"exec", IoSQLite_exec},
		{"error", IoSQLite_errorMessage},
		{"version", IoSQLite_version},
		{"setTimeoutSeconds", IoSQLite_setTimeoutSeconds},
		{"timeoutSeconds", IoSQLite_timeoutSeconds},
		{"rowsChangedCount", IoSQLite_changes},
		{"lastInsertRowId", IoSQLite_lastInsertRowId},
		{"tableNames", IoSQLite_tableNames},
		{"viewNames", IoSQLite_viewNames},
		{"columnNamesOfTable", IoSQLite_columnNamesOfTable},
		{"debugOn", IoSQLite_debugOn},
		{"debugOff", IoSQLite_debugOff},
		{"isOpen", IoSQLite_isOpen},
		{"escapeString", IoSQLite_escapeString},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoSQLite *IoSQLite_rawClone(IoSQLite *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoSQLiteData)));
	DATA(self)->error = NULL;
	IoSQLite_error_(self, "");
	return self;
}

/* ----------------------------------------------------------- */

IoSQLite *IoSQLite_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

IoSQLite *IoSQLite_newWithPath_(void *state, IoSymbol *path)
{
	IoSQLite *self = IoSQLite_new(state);
	DATA(self)->path = IOREF(path);
	return self;
}

void IoSQLite_free(IoSQLite *self)
{
	if (DATA(self)->db) sqlite_close(DATA(self)->db);
	if (DATA(self)->error) free(DATA(self)->error);
	io_free(IoObject_dataPointer(self));
}

void IoSQLite_mark(IoSQLite *self)
{
	IoObject_shouldMark((IoObject *)DATA(self)->path);

	if (DATA(self)->results)
	{
		IoObject_shouldMark((IoObject *)DATA(self)->results);
	}
}

void IoSQLite_error_(IoSQLite *self, char *error)
{
	DATA(self)->error = strcpy((char *)realloc(DATA(self)->error, strlen(error)+1), error);

	if (strlen(DATA(self)->error) && DATA(self)->debugOn)
	{
		IoState_print_(IOSTATE, "*** IoSQLite error '%s' ***\n", DATA(self)->error);
	}
}

char *IoSQLite_error(IoSQLite *self)
{
	return DATA(self)->error;
}

/* ----------------------------------------------------------- */

static int IoSQLite_busyHandler(void *context, const char *s, int n)
{
	IoSQLite *self = context;
	IoState_yield(IOSTATE);
	return 1;
}

IoObject *IoSQLite_path(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite path
	Returns the path to the database file. 
	*/
	
	return DATA(self)->path;
}

IoObject *IoSQLite_setPath(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite setPath(aSeq)
	Sets the path to the database file. Returns self. 
	*/

	DATA(self)->path = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	return self;
}

IoObject *IoSQLite_timeoutSeconds(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite timeoutSeconds
	Returns the number of seconds to wait before timing out an open call. 
	If the number is 0, an open call will never timeout. 
	*/
	
	return IONUMBER(DATA(self)->timeoutSeconds);
}

IoObject *IoSQLite_setTimeoutSeconds(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite setTimeoutSeconds(aNumber)
	Sets the open timeout to aNumber. If aNumber is 0, an open 
	call will never timeout. Returns self. 
	*/

	IoNumber *num = IoMessage_locals_numberArgAt_(m, locals, 0);
	IOASSERT(IoNumber_asDouble(num) >= 0, "SQLite timeout must be a positive number");
	DATA(self)->timeoutSeconds = IoNumber_asDouble(num);
	return self;
}

IoObject *IoSQLite_open(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite open(optionalPathString)
	Opens the database.Returns self on success or nil upon failure.
	If the databse is locked, "yield" will be called until it is 
	accessable or timeoutSeconds has expired.
	*/

	char *zErrMsg;

	if (DATA(self)->debugOn)
	{
		IoState_print_(IOSTATE, "IoSQLite opening '%s'\n", CSTRING(DATA(self)->path));
	}

	DATA(self)->db = sqlite_open(CSTRING(DATA(self)->path), 0, &zErrMsg);

	if (!DATA(self)->db)
	{
		IoSQLite_error_(self, zErrMsg);
	}
	else
	{
		IoSQLite_error_(self, "");
	}

	sqlite_busy_handler(DATA(self)->db, IoSQLite_busyHandler, self);
	sqlite_busy_timeout(DATA(self)->db, DATA(self)->timeoutSeconds*1000);
	return self;
}

IoObject *IoSQLite_isOpen(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite isOpen
	Returns true if the database is open, false otherwise.
	*/

	return IOBOOL(self, DATA(self)->db != NULL);
}

IoObject *IoSQLite_close(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite close
	Closes the database if it is open. Returns self. 
	If the database is open when the open is garbage collected, 
	it will be automatically closed. 
	*/

	if (DATA(self)->db)
	{
		sqlite_close(DATA(self)->db);
		DATA(self)->db = NULL;
	}

	return self;
}

static int IoSQLite_singleItemResultRow(void *context, int argc, char **argv, char **azColName)
{
	IoSQLite *self = context;
	int i = 0;
	IoSymbol *value;

	if (argv[i])
	{
		value = IOSYMBOL(argv[i]);
	}
	else
	{
		value = IOSYMBOL((char *)"NULL");
	}

	IoList_rawAppend_(DATA(self)->results, value);

	return 0;
}

static int IoSQLite_columnNamesResultRow(void *context, int argc, char **argv, char **azColName)
{
	IoSQLite *self = context;
	int i = 0;

	for (i= 0; i < argc; i ++)
	{
		if (!strcmp(azColName[i], "name"))
		{
			IoList_rawAppend_(DATA(self)->results, IOSYMBOL(argv[i]));
			break;
		}
	}

	return 0;
}

static int IoSQLite_resultRow(void *context, int argc, char **argv, char **azColName)
{
	IoSQLite *self = context;
	IoState_pushRetainPool(IOSTATE);

	{
		IoMap *map = IoMap_new(IOSTATE);
		PHash *hash = IoMap_rawHash(map);
		int i;
		IoSymbol *key, *value;

		for(i = 0; i < argc; i ++)
		{
			key = IOSYMBOL(azColName[i]);

			if (argv[i])
			{
				value = IOSYMBOL(argv[i]);
			}
			else
			{
				value = IOSYMBOL((char *)"NULL");
			}

			PHash_at_put_(hash, key, value);
			/*printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL"); */
		}

		IoList_rawAppend_(DATA(self)->results, map);
	}

	IoState_popRetainPool(IOSTATE);

	return 0;
}

IoObject *IoSQLite_execWithCallback(IoSQLite *self,
									IoObject *locals,
									IoMessage *m,
									IoSymbol *s,
									ResultRowCallback *callback)
{
	IoList *results;

	if (!DATA(self)->db)
	{
		IoSQLite_open(self, locals, m);

		if (!DATA(self)->db)
		{
			return IONIL(self);
		}
	}

	DATA(self)->results = IOREF(IoList_new(IOSTATE));

	if (DATA(self)->debugOn)
	{
		IoState_print_(IOSTATE, "*** %s ***\n", CSTRING(s));
	}

	{
		char *zErrMsg;
		int rc = sqlite_exec(DATA(self)->db, CSTRING(s), callback, self, &zErrMsg);

		if (rc != SQLITE_OK)
		{
			IoSQLite_error_(self, zErrMsg);
			IoState_error_(IOSTATE, m, zErrMsg);
		}
		else
		{
			IoSQLite_error_(self, "");
		}
	}

	results = DATA(self)->results;
	DATA(self)->results = NULL;
	return results;
}

IoObject *IoSQLite_exec(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite exec(aString)
	Opens the database if it is not already open and executes
	aString as an sql command. Results a List of Map objects or Nil if
	there was an error. Each map holds the contents of a row.
	The key/value pairs of the maps being column name/column value
	pairs for a row. ")
	*/

	IoSymbol *s = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IoSQLite_execWithCallback(self, locals, m, s, IoSQLite_resultRow);
}

IoObject *IoSQLite_errorMessage(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite error
	Results a string containing the current error. 
	If there is no error, Nil is returned. 
	*/

	if (strlen(DATA(self)->error)==0)
	{
		return IONIL(self);
	}

	return IOSYMBOL(DATA(self)->error);
}

IoObject *IoSQLite_version(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite version
	Results a string the version of SQLite being used. 
	*/

	return IOSYMBOL(SQLITE_VERSION);
}

IoObject *IoSQLite_changes(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite changes
	Returns the number of rows that were changed by the most 
	recent SQL statement. Or Nil if the database is closed.
	*/

	if (!DATA(self)->db)
	{
		return IONUMBER(0);
	}

	return IONUMBER(sqlite_changes(DATA(self)->db));
}

IoObject *IoSQLite_lastInsertRowId(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite lastInsertRowId
	Returns the number with the row id of the last row inserted. 
	*/

	if (!DATA(self)->db)
	{
		return IONIL(self);
	}

	return IONUMBER(sqlite_last_insert_rowid(DATA(self)->db));
}

IoObject *IoSQLite_tableNames(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite tableNames
	Returns a list containing the names of all tables in the database.
	*/

	IoSymbol *s = IOSYMBOL("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
	return IoSQLite_execWithCallback(self, locals, m, s, IoSQLite_singleItemResultRow);
}

IoObject *IoSQLite_viewNames(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite viewNames
	Returns a list containing the names of all views in the database.
	*/

	IoSymbol *s = IOSYMBOL("SELECT name FROM sqlite_master WHERE type='view' ORDER BY name");
	return IoSQLite_execWithCallback(self, locals, m, s, IoSQLite_singleItemResultRow);
}

IoObject *IoSQLite_columnNamesOfTable(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite columnNamesOfTable(tableName)
	Returns a list containing the names of all columns in the specified table.
	*/

	IoSymbol *tableName = IoMessage_locals_symbolArgAt_(m, locals, 0);
	IoSymbol *s = IoSeq_newSymbolWithFormat_(IOSTATE, "PRAGMA TABLE_INFO(%s)", CSTRING(tableName));
	return IoSQLite_execWithCallback(self, locals, m, s, IoSQLite_columnNamesResultRow);
}

IoObject *IoSQLite_debugOn(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite debugOn
	Turns on debugging.
	*/

	DATA(self)->debugOn = 1;
	return self;
}

IoObject *IoSQLite_debugOff(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite debugOff
	Turns off debugging.
	*/

	DATA(self)->debugOn = 0;
	return self;
}

IoObject *IoSQLite_escapeString(IoSQLite *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite escapeString(aString)
	Returns a translated version of aString by making two 
	copies of every single-quote (') character. This has the effect 
	of escaping the end-of-string meaning of single-quote within a string literal.
	*/

	IoSymbol *s = IoMessage_locals_seqArgAt_(m, locals, 0);
	char *newString = sqlite_mprintf("%q", CSTRING(s));
	UArray *ba = UArray_newWithCString_(newString);
	sqlite_freemem(newString);
	return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}
