/*#io
MySQL ioDoc(
	docCopyright("Min-hee Hong", 2007)
	docLicense("MIT License")
	docObject("MySQL")
	docDescription("""<a href="http://www.mysql.com/">MySQL</a> is a fast, multi-threaded, multi-user SQL database server. IoMySQL is a MySQL binding for Io, by <a href="http://dahlia.pe.kr/">Min-hee Hong</a>.

<pre><code>
my := MySQL establish(&quot;localhost&quot;, &quot;user&quot;, &quot;password&quot;, &quot;database&quot;)

# Get rows by Map
my queryThenMap(&quot;SELECT * FROM rel&quot;) foreach(at(&quot;col&quot;) println)
# Get rows by List
my query(&quot;SELECT * FROM rel&quot;) foreach(at(0) println)

my close
</code></pre>
""")
	docCategory("Databases")
	*/

#include "IoMySQL.h"
#include "IoMessage.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoList.h"
#include "IoMap.h"

#define DATA(self) ((IoMySQLData*) IoObject_dataPointer(self))

IoTag* IoMySQL_newTag(void* state) {
	IoTag* tag = IoTag_newWithName_("MySQL");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc*) IoMySQL_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc*) IoMySQL_rawClone);
	return tag;
}

IoObject* IoMySQL_proto(void* state) {
	IoObject* self = IoObject_new(state);
	IoObject_tag_(self, IoMySQL_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoMySQLData)));
	
	IoState_registerProtoWithFunc_(state, self, IoMySQL_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"establish", IoMySQL_establish},
			{"connect", IoMySQL_connect},
			{"connected", IoMySQL_connected},
			{"close", IoMySQL_close},
			{"query", IoMySQL_query},
			{"lastInsertRowId", IoMySQL_lastInsertRowId},
			{NULL, NULL}
		};

		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoObject* IoMySQL_rawClone(IoObject* proto) {
	IoObject* self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoMySQLData)));
	return self; 
}

IoObject* IoMySQL_new(void* state) {
	IoObject* proto = IoState_protoWithInitFunction_(state, IoMySQL_proto);
	return IOCLONE(proto);
}

void IoMySQL_free(IoObject* self) {
	if(DATA(self)->connected)
		mysql_close(&DATA(self)->connection);
	free(IoObject_dataPointer(self)); 
}

/* ----------------------------------------------------------- */

IoObject* IoMySQL_establish(IoObject* self, IoObject* locals, IoMessage* m) {
	/*#io
	docSlot("establish", 
		   "Establish a connection to a MySQL database.")
	*/
	IoObject* result = IoMySQL_new(IOSTATE);
	IoMySQL_connect(result, locals, m);
	return result;
}

IoObject* IoMySQL_connect(IoObject* self, IoObject* locals, IoMessage* m) {
	IoObject *host = NULL, *user = NULL, *password = NULL, *database = NULL, *port = NULL, *socket = NULL, *ssl = NULL;

	/*#io
	docSlot("connect(host, user, password, database, port, unixSocket, useSSL)", 
		   "Connect to a MySQL database.")
	*/

	switch(IoMessage_argCount(m)) {
		case 7: ssl = IoMessage_locals_quickValueArgAt_(m, locals, 6);
		case 6: socket = IoMessage_locals_quickValueArgAt_(m, locals, 5);
		case 5: port = IoMessage_locals_quickValueArgAt_(m, locals, 4);
		case 4: database = IoMessage_locals_quickValueArgAt_(m, locals, 3);
		case 3: password = IoMessage_locals_quickValueArgAt_(m, locals, 2);
		case 2: user = IoMessage_locals_quickValueArgAt_(m, locals, 1);
		case 1: host = IoMessage_locals_quickValueArgAt_(m, locals, 0);
	}	

	if(DATA(self)->connected)
		mysql_close(&DATA(self)->connection);

	if(mysql_real_connect(
		&DATA(self)->connection,
		host && ISSEQ(host) ? IoSeq_asCString(host) : NULL,
		user && ISSEQ(user) ? IoSeq_asCString(user) : NULL,
		password && ISSEQ(password) ? IoSeq_asCString(password) : NULL,
		database && ISSEQ(database) ? IoSeq_asCString(database) : NULL,
		port && ISNUMBER(port) ? (unsigned) IoNumber_asInt(port) : 0,
		socket && ISSEQ(socket) ? IoSeq_asCString(socket) : NULL,
		ssl && ISFALSE(ssl) ? 0 : CLIENT_SSL
	)) {
		DATA(self)->connected = 1;

		IoObject_setSlot_to_(self, IOSYMBOL("host"), host ? host : IONIL(self));
		IoObject_setSlot_to_(self, IOSYMBOL("user"), user ? user : IONIL(self));
		IoObject_setSlot_to_(self, IOSYMBOL("password"), password ? password : IONIL(self));
		IoObject_setSlot_to_(self, IOSYMBOL("database"), database ? database : IONIL(self));
		IoObject_setSlot_to_(self, IOSYMBOL("port"), port ? port : IONIL(self));
		IoObject_setSlot_to_(self, IOSYMBOL("socket"), socket ? socket : IONIL(self));
		IoObject_setSlot_to_(self, IOSYMBOL("usingSSL"), ssl ? IOBOOL(self, ISTRUE(ssl)) : IOFALSE(self));
	}
	else
		IoState_error_(IOSTATE, m, "connection error(%d): %s", mysql_errno(&DATA(self)->connection), mysql_error(&DATA(self)->connection));

	return self;
}

IoObject* IoMySQL_connected(IoObject* self, IoObject* locals, IoMessage* m) {
	return IOBOOL(self, DATA(self)->connected);
}

IoObject* IoMySQL_close(IoObject* self, IoObject* locals, IoMessage* m) {
	/*#io
	docSlot("close", 
		   "Closes a previously opened connection.")
	*/

	if(DATA(self)->connected)
		mysql_close(&DATA(self)->connection);

	IoObject_removeSlot_(self, IOSYMBOL("host"));
	IoObject_removeSlot_(self, IOSYMBOL("user"));
	IoObject_removeSlot_(self, IOSYMBOL("password"));
	IoObject_removeSlot_(self, IOSYMBOL("database"));
	IoObject_removeSlot_(self, IOSYMBOL("port"));
	IoObject_removeSlot_(self, IOSYMBOL("socket"));
	IoObject_removeSlot_(self, IOSYMBOL("usingSSL"));

	return self;
}

IoObject* IoMySQL_query(IoObject* self, IoObject* locals, IoMessage* m) {
	IoObject* queryString;
	bool useMap;

	MYSQL* conn = &DATA(self)->connection;
	MYSQL_RES* result;
	MYSQL_ROW row;
	MYSQL_FIELD* column;
	char** columnNames;
	unsigned c, colLength;
	unsigned long* colLengths;
	IoObject *list, *rowObject, *tmpObject;

	if(IoMessage_argCount(m) < 1 || !ISSEQ(queryString = IoMessage_locals_quickValueArgAt_(m, locals, 0)))
		IoState_error_(IOSTATE, m, "argument 0 to method 'query' must be a Sequence");

	useMap = IoMessage_argCount(m) > 1 && ISTRUE(IoMessage_locals_quickValueArgAt_(m, locals, 1));

	if(!DATA(self)->connected)
		IoState_error_(IOSTATE, m, "not connected yet");
	
	if(mysql_real_query(conn, CSTRING(queryString), IOSEQ_LENGTH(queryString)))
		IoState_error_(IOSTATE, m, "query error(%d): %s", mysql_errno(&DATA(self)->connection), mysql_error(&DATA(self)->connection));
	
	if((result = mysql_store_result(conn)) && (colLength = mysql_num_fields(result))) {
		list = IoList_new(IOSTATE);

		if(useMap) {
			columnNames = (char**) malloc(colLength * sizeof(char*));
			for(c = 0; c < colLength && (column = mysql_fetch_field(result)); ++c)
				columnNames[c] = column->name;

			while(row = mysql_fetch_row(result)) {
				colLengths = mysql_fetch_lengths(result);
				rowObject = IoMap_new(IOSTATE);

				for(c = 0; c < colLength; ++c)
					IoMap_rawAtPut(rowObject, IOSYMBOL(columnNames[c]), IOSEQ((unsigned char *)row[c], (size_t)colLengths[c]));

				IoList_rawAppend_(list, rowObject);
			}

			free(columnNames);
		}
		else {
			while(row = mysql_fetch_row(result)) {
				colLengths = mysql_fetch_lengths(result);
				rowObject = IoList_new(IOSTATE);

				for(c = 0; c < colLength; ++c)
					IoList_rawAppend_(rowObject, IOSEQ((unsigned char *)row[c], (size_t)colLengths[c]));

				IoList_rawAppend_(list, rowObject);
			}
		}

		mysql_free_result(result);
		return list;
	}
	else
		return IONUMBER(mysql_affected_rows(conn));
}

IoObject* IoMySQL_lastInsertRowId(IoObject* self, IoObject* locals, IoMessage* m) {
	/*#io
	docSlot("lastInsertRowId",
		"Returns the value generated for an AUTO_INCREMENT column by the previous INSERT or UPDATE statement.")
	*/

	if(DATA(self)->connected)
		return IONUMBER(mysql_insert_id(&DATA(self)->connection));
	else
		return IOSTATE->ioNil;
}

