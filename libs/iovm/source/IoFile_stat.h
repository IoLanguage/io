/*   
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOSEQ_FNMATCH_DEFINED
#define IOSEQ_FNMATCH_DEFINED 1

#include "IoFile.h"

void IoFile_statInit(IoFile *self);

IoObject *IoFile_stat(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_protectionMode(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_lastAccessDate(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_lastInfoChangeDate(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_lastDataChangeDate(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_userId(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_groupId(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_statSize(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_isDirectory(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_isPipe(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_isLink(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_isRegularFile(IoFile *self, IoObject *locals, IoMessage *m);
IoObject *IoFile_isSocket(IoFile *self, IoObject *locals, IoMessage *m);

IoObject *IoFile_isUserExecutable(IoFile *self, IoObject *locals, IoMessage *m);

#endif
