/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

//#include "List.h"
#include "IoState.h"

#ifdef _WIN32
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GlU32.lib")
#endif

void IoMathInit(IoState *self, IoObject *context);
void IoFreeTypeInit(IoState *self, IoObject *context);
void IoOpenGLInit(IoState *self, IoObject *context);
void IoImageInit(IoState *self, IoObject *context);

void IoAppInit(IoState *self, IoObject *context)
{  
	IoObject *app;
  IoState_doCString_(self, "Lobby Protos IoApp := Object clone");
  IoState_doCString_(self, "Lobby Protos IoApp proto := Lobby Protos IoVM");
  IoState_doCString_(self, "Lobby Protos proto := Lobby Protos IoApp");
  
/*  IoState_doCString_(self, "autoImportResources := method(\
      dir := Directory clone;\
      path := Lobby launchPath;\
      while (path length > 1,\
	if (Directory setPath(path appendPath(\"IoResources\")) exists) then (\
	  path = path appendPath(\"IoResources\");\
	  Importer addSearchPath(launchPath);\
	  Importer addSearchPath(path appendPath(\"Interface/Ion\"));\
          Importer addSearchPath(path appendPath(\"Interface/Ion/Widgets\"));\
	  ResourceManager;\
          FontManager addPath(path appendPath(\"Library/Fonts\"));\
          ImageManager addPath(path appendPath(\"Library/Themes/Neos\"));\
	  return path;\
      	);\
	path = path removeLastPathComponent;\
      )\
      Nil);"); */
  
	app = (IoObject *)IoState_doCString_(self, "Lobby Protos IoApp");

	IoMathInit(self, app);
	IoOpenGLInit(self, app);
	IoImageInit(self, app);
	IoFreeTypeInit(self, app);
}

