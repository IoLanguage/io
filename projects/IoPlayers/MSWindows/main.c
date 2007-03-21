/*   Copyright (c) 2002, Steve Dekorte
 *   All rights reserved. See _License.txt.
 */

#include "IoState.h"
#include <windows.h>

void IoDesktopInit(IoState *self);

void errorCallback( void *state, char *exception, char *description )
{
  MessageBox( 0, description, "Io Exception", MB_OK );
}

int main(int argc, const char *argv[])
{
  IoState *state = IoState_new();
  int v;
  IoState_pauseGarbageCollector(state);
  IoDesktopInit(state);
  
  IoObject_setSlot_to_(state->lobby, 
    IoState_stringWithCString_(state, "distribution"), 
    IoState_stringWithCString_(state, "IoPlayer"));

  IoState_errorCallback_( state, errorCallback );

  IoState_resumeGarbageCollector(state);
  v = IoState_main(state, argc, argv);
  /*IoState_free(state);*/
  return v;
}
