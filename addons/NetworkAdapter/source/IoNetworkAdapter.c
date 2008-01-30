/*#io
NetworkAdapter ioDoc(
    docCopyright("Rich Collins", 2007)
    docLicense("BSD revised")
    docCategory("Networking")
    docDescription("Interface to network adapter functionality")
*/

#include "IoNetworkAdapter.h"
#include "IoState.h"
#include "IoSeq.h"
#include "OSXMacAddress.h"
#include "WindowsMacAddress.h"

IoTag *IoNetworkAdapter_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("NetworkAdapter");
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
	IoNetworkAdapter *proto = IoState_protoWithInitFunction_(state, IoNetworkAdapter_proto);
	return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

IoSeq *IoNetworkAdapter_macAddress(IoNetworkAdapter *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("macAddress", "Returns the MAC address for the primary network adapter in a Hex string.")
	*/

#if defined(WIN32) ||  defined(__APPLE__) || defined(MACOSX)
	char * mac = macAddress();
	return mac ? IOSYMBOL(mac) : IONIL(self);
#else
	return IONIL(self);
#endif

}