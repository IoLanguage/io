//metadoc User copyright Steve Dekorte, 2004
//metadoc User license BSD revised

/*metadoc User description
	This object provides access to the local operating system's information 
	about the current user.
*/

//metadoc User credits Windows code by Mike Austin
//metadoc User category Server


#include "IoUser.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoDirectory.h"
#include <stdlib.h>

static const char *protoId = "User";

IoTag *IoUser_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoUser_rawClone);
	return tag;
}

IoUser *IoUser_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoUser_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"name", IoUser_protoName},
		{"homeDirectory", IoUser_homeDirectory},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoUser *IoUser_rawClone(IoUser *proto)
{
	IoUser *self = IoObject_rawClonePrimitive(proto);
	return self;
}

/* ----------------------------------------------------------- */

IoUser *IoUser_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

#ifdef WIN32

#include <windows.h>
#include <shlobj.h>

IoObject *IoUser_protoName(IoUser *self, IoObject *locals, IoMessage *m)
{
	TCHAR userName[256]; DWORD userSize = 255;
	/* Copies up to userSize, then sets userSize to actual size */
	GetUserName(userName, &userSize);
	return IOSYMBOL(userName);
}

IoObject *IoUser_homeDirectory(IoUser *self, IoObject *locals, IoMessage *m)
{
	TCHAR homePath[MAX_PATH];
	SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, homePath );
	return IoDirectory_newWithPath_(IOSTATE, IOSYMBOL(homePath));
}

#else /* Unix */

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

IoObject *IoUser_protoName(IoUser *self, IoObject *locals, IoMessage *m)
{
	/*doc User name
	Returns the current user's name.
	*/

	char *userName = (char *)getlogin();

	if (userName == NULL)
	{
		userName = getenv("LOGNAME");
	}

	if (userName == NULL)
	{
		return IONIL(self);
	}

	return IOSYMBOL(userName);
}

#define IODIRECTORY(path) IoDirectory_newWithPath_(IOSTATE, IOSYMBOL(path));

IoObject *IoUser_homeDirectory(IoUser *self, IoObject *locals, IoMessage *m)
{
	/*doc User homeDirectory
	Returns the current user's home directory as a Directory object.
*/

	char *login = (char *)getlogin();
	//IoSymbol *homePath;

	if (login)
	{
		struct passwd *pw = getpwnam(login);

		if (pw && pw->pw_dir)
		{
			return IODIRECTORY(pw->pw_dir);
		}
	}

	{
		char *path = getenv("HOME");

		if (path)
		{
			return IODIRECTORY(path);
		}
		else
		{
			return IODIRECTORY("~");
		}
	}
}

#endif

