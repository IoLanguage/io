#include <IoState.h>
#include <IoObject.h>
#include <IoCLI.h>

#include "IoAppInit.h"

int main(int argc, const char *argv[])
{
    IoState *self = IoState_new();
    
    IoState_argc_argv_(self, argc, argv);
    
    IoState_pauseGarbageCollector(self);
    IoAppInit(self, self->lobby);
    IoObject_setSlot_to_(self->lobby, SIOSTRING("distribution"), SIOSTRING("IoApp"));
    IoState_resumeGarbageCollector(self);
    IoState_clearRetainStack(self);
    
    {
	IoCLI *cli = IoCLI_new();
	IoCLI_state_(cli, self);
	IoCLI_run(cli);
	IoCLI_free(cli);
    }
    
    IoState_free(self);

	//getch();

    return 0; 
}