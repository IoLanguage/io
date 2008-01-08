/*#io
DBI ioDoc(
	docCopyright("Jeremy Cowgar", 2006)
	docLicense("BSD revised")
	docCategory("Databases")
	docObject("DBI")
	docDescription("An object for wrapping libdbi to allow Io access to
a multitude of database systems transparently.")
*/

#include "IoDBI.h"
#include "IoDBIConn.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoMap.h"

#define DATA(self) ((IoDBIData *)IoObject_dataPointer(self))

void ReportDBIError(dbi_conn conn, void *state, IoMessage *m)
{
	const char *error;
	int errorCode = dbi_conn_error(conn, &error);

	IoState_error_(state, m, "libdbi: %i: %s\n", errorCode, error);
}

IoTag *IoDBI_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("DBI");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoDBI_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoDBI_free);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoDBI_mark);
	return tag;
}

IoDBI *IoDBI_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoDBI_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoDBIData)));
	DATA(self)->driverCount = 0;

	IoState_registerProtoWithFunc_(state, self, IoDBI_proto);

	{
		IoMethodTable methodTable[] = {
			{"init", IoDBI_init},
			{"initWithDriversPath", IoDBI_initWithDriversPath},
			{"version", IoDBI_version},
			{"drivers", IoDBI_drivers},
			{"with", IoDBI_with},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoDBI *IoDBI_rawClone(IoDBI *proto)
{
	IoDBI *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoDBIData)));
	DATA(self)->driverCount = 0;
	return self;
}

void IoDBI_free(IoDBI *self)
{
	dbi_shutdown();
	free(IoObject_dataPointer(self));
}

//void IoDBI_mark(IoDBI *self) {}

/* ---------------------------------------------------------------- */

IoObject *IoDBI_version(IoDBI *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("version", "Return string version of libdbi being used")
	*/
	return IOSYMBOL(dbi_version());
}

IoObject *IoDBI_init(IoDBI *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("init", "Initialize the DBI environment with the default driver path")
	*/
	DATA(self)->driverCount = dbi_initialize(NULL);
	if (DATA(self)->driverCount == -1)
	{
		IoState_error_(IOSTATE, m, "*** IoDBI error during dbi_initialize\n");
	}
	else
	{
		DATA(self)->didInit = 1;
	}

	return IONUMBER(DATA(self)->driverCount);
}

IoObject *IoDBI_initWithDriversPath(IoDBI *self, IoObject *locals,
			IoMessage *m)
{
	/*#io
	docSlot("initWithDriversPath", "Initialize the DBI environment with the
	specified libdbi driver path")
	*/
	IoObject *dir = IoMessage_locals_valueArgAt_(m, locals, 0);

	if (ISSYMBOL(dir))
	{
		DATA(self)->driverCount = dbi_initialize(CSTRING(dir));
	}
	else
	{
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Symbol, not a '%s'\n",
			CSTRING(IoMessage_name(m)), IoObject_name(dir));
	}

	if (DATA(self)->driverCount == -1)
	{
		IoState_error_(IOSTATE, m, "*** IoDBI error during dbi_initialize\n");
	}
	else
	{
		DATA(self)->didInit = 1;
	}

	return IONUMBER(DATA(self)->driverCount);
}

IoObject *IoDBI_drivers(IoDBI *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("drivers", "Get a list of drivers and it's associated information:

	<ol>
		<li>name</li>
		<li>description</li>
		<li>filename</li>
		<li>version</li>
		<li>date compiled</li>
		<li>maintainer</li>
		<li>url</li>
	</ol>")
	*/
	IoList *list = IOREF(IoList_new(IOSTATE));
	dbi_driver driver = NULL;

	while((driver = dbi_driver_list(driver)) != NULL)
	{
		IoList *dlist = IOREF(IoList_new(IOSTATE));
		IoList_rawAppend_(dlist, IOSYMBOL(dbi_driver_get_name(driver)));
		IoList_rawAppend_(dlist, IOSYMBOL(dbi_driver_get_description(driver)));
		IoList_rawAppend_(dlist, IOSYMBOL(dbi_driver_get_filename(driver)));
		IoList_rawAppend_(dlist, IOSYMBOL(dbi_driver_get_version(driver)));
		IoList_rawAppend_(dlist, IOSYMBOL(dbi_driver_get_date_compiled(driver)));
		IoList_rawAppend_(dlist, IOSYMBOL(dbi_driver_get_maintainer(driver)));
		IoList_rawAppend_(dlist, IOSYMBOL(dbi_driver_get_url(driver)));

		IoList_rawAppend_(list, dlist);
	}

	return list;
}

IoObject *IoDBI_with(IoDBI *self, IoObject *locals, IoMessage *m)
{
	/*#io
		docSlot("with(driverName)", "Get a new connection with the given driver")
	*/
	IoObject *name = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (!ISSYMBOL(name))
	{
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a Symbol, not a '%s'\n",
			CSTRING(IoMessage_name(m)), IoObject_name(name));
		return IONIL(self);
	}

	if (DATA(self)->didInit != 1)
	{
		IoDBI_init(self, locals, m);
	}

	dbi_conn c = dbi_conn_new(CSTRING(name));
	if (c == NULL)
	{
		IoState_error_(IOSTATE, m, "libdbi error during dbi_conn_new\n");
		return IONIL(self);
	}

	return IoDBIConn_new(IOSTATE, c);
}
