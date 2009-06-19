//metadoc SQLite3 copyright Steve Dekorte, 2004
//metadoc SQLite3 license BSD revised

/*metadoc SQLite3 description
	SQLite provides a embedded simple and fast 
	(2x faster than PostgreSQL or MySQL) SQL database. 
	See http://www.hwaci.com/sw/sqlite/ for details. 
	It's SQL command set is described 
	at http://www.hwaci.com/sw/sqlite/lang.html. 
	SQLite was written by Dr. Richard Hipp who offers consulting 
	services for custom modifications and support of SQLite. Example:
	<p>
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

/*metadoc SQLite3 category
	Databases
*/

#include "IoSQLite3.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoMap.h"

typedef int (ResultRowCallback)(void *, int , char **, char **);

#define DATA(self) ((IoSQLite3Data *)IoObject_dataPointer(self))

/*
 static int IoSQLite3_resultRow(void *context, int argc, char **argv, char **azColName);
 static int IoSQLite3_busyHandler(void *context, const char *s, int n);
 */

IoTag *IoSQLite3_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("SQLite3");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSQLite3_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSQLite3_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoSQLite3_mark);
	return tag;
}

IoSQLite3 *IoSQLite3_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSQLite3_newTag(state));

	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoSQLite3Data)));
	DATA(self)->path = IOSYMBOL(".");

	IoState_registerProtoWithFunc_(state, self, IoSQLite3_proto);

	{
		IoMethodTable methodTable[] = {
		{"setPath", IoSQLite3_setPath},
		{"path", IoSQLite3_path},
		{"open", IoSQLite3_open},
		{"close", IoSQLite3_close},
		{"exec", IoSQLite3_exec},
		{"error", IoSQLite3_errorMessage},
		{"version", IoSQLite3_version},
		{"setTimeoutSeconds", IoSQLite3_setTimeoutSeconds},
		{"timeoutSeconds", IoSQLite3_timeoutSeconds},
		{"rowsChangedCount", IoSQLite3_changes},
		{"lastInsertRowId", IoSQLite3_lastInsertRowId},
		{"tableNames", IoSQLite3_tableNames},
		{"viewNames", IoSQLite3_viewNames},

		{"columnNamesOfTable", IoSQLite3_columnNamesOfTable},
		{"debugOn", IoSQLite3_debugOn},
		{"debugOff", IoSQLite3_debugOff},
		{"isOpen", IoSQLite3_isOpen},
		{"escapeString", IoSQLite3_escapeString},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoSQLite3 *IoSQLite3_rawClone(IoSQLite3 *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoSQLite3Data)));
	return self;
}

/* ----------------------------------------------------------- */

IoSQLite3 *IoSQLite3_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoSQLite3_proto);
	return IOCLONE(proto);
}

IoSQLite3 *IoSQLite3_newWithPath_(void *state, IoSymbol *path)
{
	IoSQLite3 *self = IoSQLite3_new(state);
	DATA(self)->path = IOREF(path);
	return self;
}

void IoSQLite3_free(IoSQLite3 *self)
{
	if (DATA(self)->db) sqlite3_close(DATA(self)->db);
	io_free(IoObject_dataPointer(self));
}

void IoSQLite3_mark(IoSQLite3 *self)
{
	IoObject_shouldMark((IoObject *)DATA(self)->path);

	if (DATA(self)->results)
	{
		IoObject_shouldMark((IoObject *)DATA(self)->results);
	}
}

void IoSQLite3_showError(IoSQLite3 *self)
{
	int status = sqlite3_errcode(DATA(self)->db);

	if (status != SQLITE_OK)
	{
		const char *error = IoSQLite3_error(self);

		if (DATA(self)->debugOn)
		{
			IoState_print_(IOSTATE, "*** IoSQLite3 error '%s' ***\n", error);
		}
	}
}

const char *IoSQLite3_error(IoSQLite3 *self)
{
	return sqlite3_errmsg(DATA(self)->db);
}

/* ----------------------------------------------------------- */

static int IoSQLite3_busyHandler(void *context, int n)
{
	IoSQLite3 *self = context;
	IoState_yield(IOSTATE);
	return 1;
}

IoObject *IoSQLite3_path(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 path
	Returns the path to the database file. 
	*/
	
	return DATA(self)->path;
}

IoObject *IoSQLite3_setPath(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 setPath
	Sets the path to the database file. Returns self. 
	*/

	DATA(self)->path = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	return self;
}

IoObject *IoSQLite3_timeoutSeconds(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 timeoutSeconds
	Returns the number of seconds to wait before timing out an
	open call. If the number is 0, an open call will never timeout. 
	*/
	
	return IONUMBER(DATA(self)->timeoutSeconds);
}

IoObject *IoSQLite3_setTimeoutSeconds(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 setTimeoutSeconds(aNumber)
	Sets the open timeout to aNumber. If aNumber is 0, an open
	call will never timeout. Returns self. 
	*/

	IoNumber *num = IoMessage_locals_numberArgAt_(m, locals, 0);
	IOASSERT(IoNumber_asDouble(num) >= 0, "SQLite timeout must be a positive number");
	DATA(self)->timeoutSeconds = IoNumber_asDouble(num);
	return self;
}

IoObject *IoSQLite3_open(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 open(optionalPathString)
	Opens the database. If there is an optionalPathString argument,
	the path is set to it's value before opening the database. If path is "" or ":memory:"
	a database will be created in-memory, otherwise the file specified by path is opened.
	Returns self or Nil upon failure.

	If the databse is locked, "yield" will be called until it is accessable or
	timeoutSeconds has expired. ""
	*/

	if (IoMessage_argCount(m) > 0)
	{
		DATA(self)->path = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
	}

	sqlite3_open(CSTRING(DATA(self)->path), &(DATA(self)->db));

	IoSQLite3_showError(self);

	sqlite3_busy_handler(DATA(self)->db, IoSQLite3_busyHandler, self);
	sqlite3_busy_timeout(DATA(self)->db, DATA(self)->timeoutSeconds * 1000);
	return self;
}

IoObject *IoSQLite3_isOpen(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 isOpen
	Returns true if the database is open, false otherwise.
	*/

	return IOBOOL(self, DATA(self)->db != NULL);
}

IoObject *IoSQLite3_close(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 close
	Closes the database if it is open. Returns self. If the database is open when the open is garbage collected, it will be automatically closed. 
	*/

	if (DATA(self)->db)
	{
		sqlite3_close(DATA(self)->db);
		DATA(self)->db = NULL;
	}

	return self;
}

static int IoSQLite3_singleItemResultRow(void *context, int argc, char **argv, char **azColName)
{
	IoSQLite3 *self = context;
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

static int IoSQLite3_columnNamesResultRow(void *context, int argc, char **argv, char **azColName)
{
	IoSQLite3 *self = context;
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

static int IoSQLite3_resultRow(void *context, int argc, char **argv, char **azColName)
{
	IoSQLite3 *self = context;
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

IoObject *IoSQLite3_execWithCallback(IoSQLite3 *self,
									 IoObject *locals, IoMessage *m, IoSymbol *s, ResultRowCallback *callback)
{
	IoList *results;

	if (!DATA(self)->db)
	{
		IoSQLite3_open(self, locals, m);

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
		sqlite3_exec(DATA(self)->db, CSTRING(s), callback, self, &zErrMsg);

		IoSQLite3_showError(self);
	}

	results = DATA(self)->results;
	DATA(self)->results = NULL;
	return results;
}

IoObject *IoSQLite3_exec(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 exec(aString)
	Opens the database if it is not already open and executes
	aString as an sql command. Results a List of Map objects or Nil if
	there was an error. Each map holds the contents of a row.
	The key/value pairs of the maps being column name/column value
	pairs for a row.
	*/

	IoSymbol *s = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IoSQLite3_execWithCallback(self, locals, m, s, IoSQLite3_resultRow);
}

IoObject *IoSQLite3_errorMessage(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3  error
	Results a string containing the current error. If there is no error, Nil is returned. 
	*/

	if (sqlite3_errcode(DATA(self)->db) == SQLITE_OK)
	{
		return IONIL(self);
	}

	return IOSYMBOL(IoSQLite3_error(self));
}

IoObject *IoSQLite3_version(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 version
	Results a string the version of SQLite being used. 
	*/

	return IOSYMBOL(SQLITE_VERSION);
}

IoObject *IoSQLite3_changes(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 changes
	Returns the number of rows that were changed by the most
	recent SQL statement. Or Nil if the database is closed.
	*/

	if (!DATA(self)->db)
	{
		return IONUMBER(0);
	}

	return IONUMBER(sqlite3_changes(DATA(self)->db));
}

IoObject *IoSQLite3_lastInsertRowId(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 lastInsertRowId
	Returns the number with the row id of the last row inserted. 
	*/

	if (!DATA(self)->db)
	{
		return IONIL(self);
	}

	return IONUMBER(sqlite3_last_insert_rowid(DATA(self)->db));
}

IoObject *IoSQLite3_tableNames(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 tableNames
	Returns a list containing the names of all tables in the database.
	*/

	IoSymbol *s = IOSYMBOL("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
	return IoSQLite3_execWithCallback(self, locals, m, s, IoSQLite3_singleItemResultRow);
}

IoObject *IoSQLite3_viewNames(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 viewNames
	Returns a list containing the names of all
	views in the database.
	*/

	IoSymbol *s = IOSYMBOL("SELECT name FROM sqlite_master WHERE type='view' ORDER BY name");
	return IoSQLite3_execWithCallback(self, locals, m, s, IoSQLite3_singleItemResultRow);
}

IoObject *IoSQLite3_columnNamesOfTable(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 columnNamesOfTable(tableName)
	Returns a list containing the names of all columns in the specified table.
	*/

	IoSymbol *tableName = IoMessage_locals_symbolArgAt_(m, locals, 0);
	IoSymbol *s = IoSeq_newSymbolWithFormat_(IOSTATE, "PRAGMA TABLE_INFO(%s)", CSTRING(tableName));
	return IoSQLite3_execWithCallback(self, locals, m, s, IoSQLite3_columnNamesResultRow);
}

IoObject *IoSQLite3_debugOn(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 debugOn
	Turns on debugging.
	*/

	DATA(self)->debugOn = 1;
	return self;
}

IoObject *IoSQLite3_debugOff(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 debugOff
	Turns off debugging.
	*/

	DATA(self)->debugOn = 0;
	return self;
}

IoObject *IoSQLite3_escapeString(IoSQLite3 *self, IoObject *locals, IoMessage *m)
{
	/*doc SQLite3 escapeString(aString)
	Returns a translated version of aString by making two copies of 
	every single-quote (') character. This has the effect of escaping the 
	end-of-string meaning of single-quote within a string literal.
	*/

	IoSymbol *s = IoMessage_locals_seqArgAt_(m, locals, 0);
	char *newString = sqlite3_mprintf("%q", CSTRING(s));
	UArray *ba = UArray_newWithCString_(newString);
	sqlite3_free(newString);
	return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}
