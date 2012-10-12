//metadoc NetworkAdapter copyright Rich Collins, 2007
//metadoc NetworkAdapter license BSD revised
//metadoc NetworkAdapter category Networking
/*metadoc NetworkAdapter description
Interface to network adapter functionality.
*/

#include "IoNetworkAdapter.h"
#include "IoState.h"
#include "IoSeq.h"
#include "OSXMacAddress.h"
#include "WindowsMacAddress.h"

static const char *protoId = "NetworkAdapter";

IoTag *IoNetworkAdapter_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoNetworkAdapter_rawClone);
	return tag;
}

IoNetworkAdapter *IoNetworkAdapter_proto(void *state)
{
	IoNetworkAdapter *self = IoObject_new(state);
	IoObject_tag_(self, IoNetworkAdapter_newTag(state));

	IoState_registerProtoWithFunc_(state, self, IoNetworkAdapter_proto);

	{
		IoMethodTable methodTable[] = {
			{"macAddress", IoNetworkAdapter_macAddress},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoNetworkAdapter *IoNetworkAdapter_rawClone(IoNetworkAdapter *proto)
{
	IoNetworkAdapter *self = IoObject_rawClonePrimitive(proto);
	return self;
}

IoNetworkAdapter *IoNetworkAdapter_new(void *state)
{
	IoNetworkAdapter *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

IoSeq *IoNetworkAdapter_macAddress(IoNetworkAdapter *self, IoObject *locals, IoMessage *m)
{
	/*doc NetworkAdapter macAddress
	Returns the MAC address for the primary network adapter in a Hex string.
	*/

#if defined(WIN32) ||  defined(__APPLE__) || defined(MACOSX)
	char * mac = macAddress();
	return mac ? IOSYMBOL(mac) : IONIL(self);
#else
	return IONIL(self);
#endif

}
