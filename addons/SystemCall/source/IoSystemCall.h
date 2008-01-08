/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOSystemCall_DEFINED
#define IOSystemCall_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"
#include "callsystem.h"

#define ISSYSTEMCALL(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSystemCall_rawClone)

typedef IoObject IoSystemCall;

typedef struct
{
	char **env;
	char **args;
	callsystem_fd_t stdin_child[2];
	callsystem_fd_t stdout_child[2];
	callsystem_fd_t stderr_child[2];
	callsystem_pid_t pid;
	int status;
	int needsClose;
} IoSystemCallData;

IoTag *IoSystemCall_newTag(void *state);
IoSystemCall *IoSystemCall_proto(void *state);
IoSystemCall *IoSystemCall_rawClone(IoObject *self);
void IoSystemCall_free(IoObject *self);

IoObject *IoSystemCall_asyncRun(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSystemCall_status(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSystemCall_close(IoObject *self, IoObject *locals, IoMessage *m);
void IoSystemCall_rawClose(IoSystemCall *self);

#endif
