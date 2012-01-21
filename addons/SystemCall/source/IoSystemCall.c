//metadoc SystemCall license BSD revised
//metadoc SystemCall category Server
/*metadoc SystemCall description
A binding for "callsystem - system() on steorids"

<UL>
<LI> asynchonous running of a child process
<LI> setup of the environment
<LI> substitution of environment variables
<LI> connect all 3 standard streams to pipes, null devices, or files
<LI> pathname handling
</UL>

Example use;
<pre>	
sc = SystemCall clone
</pre>	
*/


#include "IoSystemCall.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoMap.h"
#include "IoList.h"
#include "IoFile.h"
#include "callsystem.h"

static const char *protoId = "SystemCall";

#define DATA(self) ((IoSystemCallData *)IoObject_dataPointer(self))

IoTag *IoSystemCall_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSystemCall_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSystemCall_rawClone);
	return tag;
}

static void IoSystemCall_clearPipeDescriptors(IoSystemCall* self)
{
	DATA(self)->stdin_child[0] =
	DATA(self)->stdin_child[1] = CALLSYSTEM_ILG_FD;
	DATA(self)->stdout_child[0] =
	DATA(self)->stdout_child[1] = CALLSYSTEM_ILG_FD;
	DATA(self)->stderr_child[0] =
	DATA(self)->stderr_child[1] = CALLSYSTEM_ILG_FD;
}

IoSystemCall *IoSystemCall_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSystemCall_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoSystemCallData)));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"asyncRun", IoSystemCall_asyncRun},
		{"status", IoSystemCall_status},
		{"close", IoSystemCall_close},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	IoSystemCall_clearPipeDescriptors(self);
	return self;
}

IoSystemCall *IoSystemCall_rawClone(IoSystemCall *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoSystemCallData)));
	IoSystemCall_clearPipeDescriptors(self);
	return self; 
}

IoSystemCall *IoSystemCall_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IoSystemCall_rawClone(proto);
}

/* ----------------------------------------------------------- */

void IoSystemCall_free(IoSystemCall *self)
{
	IoSystemCall_rawClose(self);
	free(DATA(self));
}

/* ----------------------------------------------------------- */

IoObject *IoSystemCall_asyncRun(IoSystemCall *self, IoObject *locals, IoMessage *m)
{
	//doc SystemCall asyncRun(command, argList, envMap) Run the system call.

	IoSymbol *command = IoMessage_locals_symbolArgAt_(m, locals, 0);
	List *args = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 1));
	IoMap *envMap = IoMessage_locals_mapArgAt_(m, locals, 2);
	List *envKeys = IoList_rawList(IoMap_rawKeys(envMap));
	int err;

	/*if you want C FILE streams*/
	/* STREAMING THESE TO A SEQUENCE CAN CAUSE DEADLOCK!!!
	 * WE NEED TO FIND A WORKAROUND
	 UPDATE: this object is pointless without the streams (use Io's System system() instead)
	 so either fix the streams here or don't use this object, but don't remove this object's streams.
	*/
	FILE *fchildin;
	FILE *fchildout;
	FILE *fchilderr;

	IoSystemCall_rawClose(self);

	/*open the filehandles as pipes*/
	callsystem_pipe(DATA(self)->stdin_child);
	callsystem_pipe(DATA(self)->stdout_child);
	callsystem_pipe(DATA(self)->stderr_child);

	/*initialize the C FILE streams*/
	fchildin  = callsystem_fdopen(DATA(self)->stdin_child,  CALLSYSTEM_MODE_WRITE); /* the parent process wants to WRITE to stdin of the child */
	fchildout = callsystem_fdopen(DATA(self)->stdout_child, CALLSYSTEM_MODE_READ); /* the parent process wants to READ stdout of the child */
	fchilderr = callsystem_fdopen(DATA(self)->stderr_child, CALLSYSTEM_MODE_READ); /* the parent process wants to READ from stderr of the child */


	DATA(self)->pid = CALLSYSTEM_ILG_PID;

	LIST_FOREACH(envKeys, i, k,
		IoObject *v;
		IOASSERT(ISSEQ(k), "envKeys must be strings");
		v = IoMap_rawAt(envMap, k);
		IOASSERT(ISSEQ(v), "envValues must be strings");
		callsystem_setenv(&(DATA(self)->env), UTF8CSTRING(k), UTF8CSTRING(v));
	);

	LIST_FOREACH(args, i, arg,
		IOASSERT(ISSEQ(arg), "args must be strings");
		callsystem_argv_pushback(&DATA(self)->args, UTF8CSTRING(arg));
	);


	err = callsystem(CSTRING(command),
		DATA(self)->args,
		DATA(self)->env,

/*
		NULL,//DATA(self)->stdin_child,
		NULL,//DATA(self)->stdout_child,
		NULL,//DATA(self)->stderr_child,
*/
		DATA(self)->stdin_child,
		DATA(self)->stdout_child,
		DATA(self)->stderr_child,
		
		NULL,
		0,
		&(DATA(self)->pid));

	//printf("callsystem %s pid %i\n", CSTRING(command), DATA(self)->pid);
	DATA(self)->needsClose = 1;

	if (err != -1)
	{
		IoObject_setSlot_to_(self, IOSYMBOL("stdin"), IoFile_newWithStream_(IOSTATE, fchildin));
		IoObject_setSlot_to_(self, IOSYMBOL("stdout"), IoFile_newWithStream_(IOSTATE, fchildout));
		IoObject_setSlot_to_(self, IOSYMBOL("stderr"), IoFile_newWithStream_(IOSTATE, fchilderr));
		
		/*
		Now that we've handed the C FILE* over to the Io File
		Objects they are responsible for closing the file.
		So we must forget the "descriptors".
		 */
		IoSystemCall_clearPipeDescriptors(self);
	}

	return IONUMBER(err);
}

IoObject *IoSystemCall_status(IoSystemCall *self, IoObject *locals, IoMessage *m)
{
	int pid = DATA(self)->pid;
	int status = callsystem_running(&pid);
	DATA(self)->pid = pid;

	/*
	if (status < 255)
	{
		IoSystemCall_rawClose(self);
	}
	*/

	return IONUMBER(status);
}

IoObject *IoSystemCall_close(IoSystemCall *self, IoObject *locals, IoMessage *m)
{
	IoSystemCall_rawClose(self);
	return self;
}

void IoSystemCall_rawClose(IoSystemCall *self)
{
	//printf("IoSystemCall_rawClose(%p) 1\n", (void *)self);

	if (DATA(self)->needsClose)
	{
		//printf("IoSystemCall_rawClose(%p)\n", (void *)self);
		callsystem_close(DATA(self)->stdin_child);
		callsystem_close(DATA(self)->stdout_child);
		callsystem_close(DATA(self)->stderr_child);
		callsystem_argv_clear(&DATA(self)->args);
		callsystem_env_clear(&DATA(self)->env);
		//if (DATA(self)->pid)
		{
			//printf("callsystem_finished(%i)\n", DATA(self)->pid);
			callsystem_finished(&(DATA(self)->pid));
			DATA(self)->pid = 0;
		}

		DATA(self)->needsClose = 0;
	}
}

