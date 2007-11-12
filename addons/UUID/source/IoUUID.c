/*#io
UUID ioDoc(
    docCopyright("Jonathan Wright", 2006)
    docLicense("BSD revised")
    docCategory("Encryption")
    docDescription("Generates UUIDs/GUIDs")
*/

#include "IoUUID.h"
#include "IoState.h"
#include "IoSeq.h"

#include <uuid/uuid.h>

IoTag *IoUUID_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("UUID");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoUUID_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoUUID_rawClone);
	return tag;
}

IoUUID *IoUUID_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoUUID_newTag(state));
	
	IoState_registerProtoWithFunc_(state, self, IoUUID_proto);
	
	{
		IoMethodTable methodTable[] = {
		{"uuid", IoUUID_uuid},
		{"uuidRandom", IoUUID_uuidRandom},
		{"uuidTime", IoUUID_uuidTime},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoUUID *IoUUID_rawClone(IoUUID *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	return self; 
}

IoUUID *IoUUID_new(void *state)
{
	IoUUID *proto = IoState_protoWithInitFunction_(state, IoUUID_proto);
	return IOCLONE(proto);
}

void IoUUID_free(IoUUID *self) 
{ 
}

/* ----------------------------------------------------------- */

IoObject *IoUUID_uuid(IoUUID *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("uuid", "Returns a new uuid in string format.")
	*/

	uuid_t buf;
	char str[200];

	uuid_generate(buf);
	uuid_unparse(buf, str);

	return IOSYMBOL(str);
}

IoObject *IoUUID_uuidRandom(IoUUID *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("uuidRandom", "Returns a new random uuid (type 4) in string format.")
	*/

	uuid_t buf;
	char str[200];

	uuid_generate_random(buf);
	uuid_unparse(buf, str);

	return IOSYMBOL(str);
}

IoObject *IoUUID_uuidTime(IoUUID *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("uuidTime", "Returns a new time and mac uuid (type 1) in string format.")
	*/

	uuid_t buf;
	char str[200];

	uuid_generate_time(buf);
	uuid_unparse(buf, str);

	return IOSYMBOL(str);
}
