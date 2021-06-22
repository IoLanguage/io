// metadoc File copyright Steve Dekorte 2002
// metadoc File license BSD revised

#ifndef IOSEQ_FNMATCH_DEFINED
#define IOSEQ_FNMATCH_DEFINED 1

#include "IoFile.h"

void IoFile_statInit(IoFile *self);

IO_METHOD(IoFile, stat);

IO_METHOD(IoFile, protectionMode);
IO_METHOD(IoFile, lastAccessDate);
IO_METHOD(IoFile, lastInfoChangeDate);
IO_METHOD(IoFile, lastDataChangeDate);
IO_METHOD(IoFile, userId);
IO_METHOD(IoFile, groupId);
IO_METHOD(IoFile, statSize);

IO_METHOD(IoFile, isDirectory);
IO_METHOD(IoFile, isPipe);
IO_METHOD(IoFile, isLink);
IO_METHOD(IoFile, isRegularFile);
IO_METHOD(IoFile, isSocket);

IO_METHOD(IoFile, isUserExecutable);

#endif
