/*
docCopyright("Joe Osborn", 2007)
docLicense("BSD revised")
*/

#ifndef IOCERTIFICATE_DEFINED
#define IOCERTIFICATE_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoMessage.h"
#include <openssl/ssl.h>

#define ISCERTIFICATE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCertificate_rawClone)

typedef IoObject IoCertificate;

IoCertificate *IoMessage_locals_certificateArgAt_(IoMessage *self, IoObject *locals, int n);
IoTag *IoCertificate_newTag(void *state);
IoCertificate *IoCertificate_proto(void *state);
IoCertificate *IoCertificate_rawClone(IoCertificate *proto);
void IoCertificate_free(IoCertificate *self);
IoCertificate *IoCertificate_newWithX509_shouldFree(void *state, X509 *x509, int shouldFree);
IoCertificate *IoCertificate_version(IoCertificate *self, IoObject *locals, IoMessage *m);
IoObject *IoCertificate_nameToObject(IoObject *self, X509_NAME *xname);
IoObject *IoCertificate_issuerName(IoCertificate *self, IoObject *locals, IoMessage *m);
IoObject *IoCertificate_subjectName(IoCertificate *self, IoObject *locals, IoMessage *m);
IoObject *IoCertificate_serialNumber(IoCertificate *self, IoObject *locals, IoMessage *m);
IoObject *IoCertificate_asnTimeToDate(IoCertificate *self, ASN1_TIME *tm);
IoObject *IoCertificate_notBefore(IoCertificate *self, IoObject *locals, IoMessage *m);
IoObject *IoCertificate_notAfter(IoCertificate *self, IoObject *locals, IoMessage *m);
IoObject *IoCertificate_extensions(IoCertificate *self, IoObject *locals, IoMessage *m);
IoObject *IoCertificate_attributes(IoCertificate *self, IoObject *locals, IoMessage *m);
#endif
