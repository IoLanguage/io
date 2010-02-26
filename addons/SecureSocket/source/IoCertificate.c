//metadoc Certificate copyright Joe Osborn, 2007
//metadoc Certificate license BSD revised
//metadoc Certificate category Networking
/*metadoc Certificate description
Read-only interface to SSL X509 certificates.

*/

#include "IoCertificate.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include <openssl/x509.h>
#include "IoList.h"
#include "IoDate.h"
#include "common.h"

IoCertificate *IoMessage_locals_certificateArgAt_(IoMessage *self, IoObject *locals, int n)
{
	IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
	
	if (!ISCERTIFICATE(v)) 
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Certificate");
	}
	
	return v;
}

typedef struct {
	X509 *x509;
	int shouldFree;
} Certificate;

#define CERTIFICATE(s) ((Certificate *)IoObject_dataPointer(s))
#define X509(s) (CERTIFICATE(self)->x509)
#define SHOULDFREE(s) (CERTIFICATE(self)->shouldFree)

IoTag *IoCertificate_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Certificate");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCertificate_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCertificate_rawClone);
	
	return tag;
}

IoCertificate *IoCertificate_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	
	IoObject_tag_(self, IoCertificate_newTag(state));

	IoState_registerProtoWithFunc_((IoState *)state, self, IoCertificate_proto);
	
	IoObject_setSlot_to_(self, IOSYMBOL("PEMType"), IONUMBER(X509_FILETYPE_PEM));
	IoObject_setSlot_to_(self, IOSYMBOL("ASN1Type"), IONUMBER(X509_FILETYPE_ASN1));
	
	{
		IoMethodTable methodTable[] = {
		{"version", IoCertificate_version},
		//doc Certificate version Returns the version.
		{"serialNumber", IoCertificate_serialNumber},
		//doc Certificate serialNumber Returns the serialNumber.
		{"notBefore", IoCertificate_notBefore},
		//doc Certificate notBefore Returns the notBefore value.
		{"notAfter", IoCertificate_notAfter},
		//doc Certificate notAfter Returns the notAfter value.
		{"extensions", IoCertificate_extensions},
		//doc Certificate extensions Returns the extensions.
		{"attributes", IoCertificate_attributes},
		//doc Certificate attributes Returns the attributes.
		{"issuerName", IoCertificate_issuerName},
		//doc Certificate issuerName Returns the issuerName.
		{"subjectName", IoCertificate_subjectName},
		//doc Certificate subjectName Returns the subjectName.
		{NULL, NULL}
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(Certificate)));
	
	return self;
}

IoCertificate *IoCertificate_rawClone(IoCertificate *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, CERTIFICATE(proto));
	//need to do more work for the copy to be correct
	return self; 
}

void IoCertificate_free(IoCertificate *self) 
{
	if(SHOULDFREE(self))
	{
		X509_free(X509(self));
	}
	free(CERTIFICATE(self));
}

IoCertificate *IoCertificate_newWithX509_shouldFree(void *state, X509 *x509, int shouldFree)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoCertificate_proto);
	IoCertificate *self = IOCLONE(proto);
	Certificate *certData = calloc(1, sizeof(Certificate));
	certData->shouldFree = shouldFree;
	certData->x509 = x509;
	IoObject_setDataPointer_(self, certData);
	return self;
}

IoCertificate *IoCertificate_version(IoCertificate *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(X509_get_version(X509(self)) + 1);
}

IoObject *IoCertificate_nameToObject(IoObject *self, X509_NAME *xname)
{
	IoObject *map = IoObject_new(IoObject_state(self));
	int i;
	for(i = 0; i < X509_NAME_entry_count(xname); i++)
	{
		X509_NAME_ENTRY *entry = X509_NAME_get_entry(xname, i);
		const char *key = (const char *)OBJ_nid2ln(OBJ_obj2nid(X509_NAME_ENTRY_get_object(entry)));
		const char *value = (const char *)ASN1_STRING_data(X509_NAME_ENTRY_get_data(entry));
		IoObject_setSlot_to_(map, IOSYMBOL(key), IoSeq_newWithCString_(IoObject_state(self), value));
	}
	return map;
}

IoObject *IoCertificate_issuerName(IoCertificate *self, IoObject *locals, IoMessage *m)
{
	return IoCertificate_nameToObject(self, X509_get_issuer_name(X509(self)));
}

IoObject *IoCertificate_subjectName(IoCertificate *self, IoObject *locals, IoMessage *m)
{
	return IoCertificate_nameToObject(self, X509_get_subject_name(X509(self)));
}

IoObject *IoCertificate_serialNumber(IoCertificate *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(ASN1_INTEGER_get(X509_get_serialNumber(X509(self))));
}

IoObject *IoCertificate_asnTimeToDate(IoCertificate *self, ASN1_TIME *tm)
{
	char *v;
	int gmt=0;
	int i;
	int y=0,M=0,d=0,h=0,m=0,s=0;

	i=tm->length;
	v=(char *)tm->data;

	if (i < 10) return IONIL(self);
	if (v[i-1] == 'Z') gmt=1;
	for (i=0; i<10; i++)
		if ((v[i] > '9') || (v[i] < '0')) return IONIL(self);
	y= (v[0]-'0')*10+(v[1]-'0');
	if (y < 50) y+=100;
	M= (v[2]-'0')*10+(v[3]-'0');
	if ((M > 12) || (M < 1)) return IONIL(self);
	d= (v[4]-'0')*10+(v[5]-'0');
	h= (v[6]-'0')*10+(v[7]-'0');
	m=  (v[8]-'0')*10+(v[9]-'0');
	if (	(v[10] >= '0') && (v[10] <= '9') &&
		(v[11] >= '0') && (v[11] <= '9'))
		s=  (v[10]-'0')*10+(v[11]-'0');
	struct tm ctm;
	ctm.tm_sec = s;
	ctm.tm_min = m;
	ctm.tm_hour = h;
	ctm.tm_mday = d;
	ctm.tm_mon = M-1;
	ctm.tm_year = y;
	ctm.tm_gmtoff = 0;
		
	return IoDate_newWithTime_(IoObject_state(self), timegm(&ctm));
}

IoObject *IoCertificate_notBefore(IoCertificate *self, IoObject *locals, IoMessage *m)
{
	return IoCertificate_asnTimeToDate(self, X509_get_notBefore(X509(self)));
}

IoObject *IoCertificate_notAfter(IoCertificate *self, IoObject *locals, IoMessage *m)
{
	return IoCertificate_asnTimeToDate(self, X509_get_notAfter(X509(self)));
}

IoObject *IoCertificate_extensions(IoCertificate *self, IoObject *locals, IoMessage *m)
{
	IoObject *map = IoObject_new(IoObject_state(self));
	int i;
	for(i = 0; i < X509_get_ext_count(X509(self)); i++)
	{
		IoObject *ioext = IoObject_new(IoObject_state(self));
		X509_EXTENSION *ext = X509_get_ext(X509(self), i);
		const char *key = (const char *)OBJ_nid2ln(OBJ_obj2nid(X509_EXTENSION_get_object(ext)));
		const char *value = (const char *)ASN1_STRING_data(X509_EXTENSION_get_data(ext));
		int isCritical = X509_EXTENSION_get_critical(ext);
		IoObject_setSlot_to_(ioext, IOSYMBOL("value"), IoSeq_newWithCString_(IoObject_state(self), value));
		IoObject_setSlot_to_(ioext, IOSYMBOL("isCritical"), IONUMBER(isCritical));
		IoObject_setSlot_to_(map, IOSYMBOL(key), ioext);
	}
	return map;
}
IoObject *IoCertificate_attributes(IoCertificate *self, IoObject *locals, IoMessage *m)
{
	IoObject *map = IoObject_new(IoObject_state(self));
	const EVP_PKEY *pkey = X509_extract_key(X509(self));
	int i;
	for(i = 0; i < EVP_PKEY_get_attr_count(pkey); i++)
	{
		IoList *list = IoList_new(IoObject_state(self));
		X509_ATTRIBUTE *attr = EVP_PKEY_get_attr(pkey, i);
		const char *key = (const char *)OBJ_nid2ln(OBJ_obj2nid(X509_ATTRIBUTE_get0_object(attr)));
		int j;
		for(j = 0; j < X509_ATTRIBUTE_count(attr); j++)
		{
			ASN1_TYPE *attrType = X509_ATTRIBUTE_get0_type(attr, j);
			ASN1_OBJECT *attrData = X509_ATTRIBUTE_get0_data(attr, j, attrType->type, NULL);
			//consider switching on attrType instead; 
			//really, that would be wiser, so that dates, 
			//numbers, etc can be happy
			/*
			switch(attrType->type) {
				case V_ASN1_OCTET_STRING:
			...
			*/
			int len = i2t_ASN1_OBJECT(NULL, 0, attrData);
			char *value = calloc(len, sizeof(char));
			i2t_ASN1_OBJECT(value, len, attrData);
			IoList_rawAppend_(list, IoSeq_newWithCString_(IoObject_state(self), value));
		}
		IoObject_setSlot_to_(map, IOSYMBOL(key), list);
	}
	return map;
}
