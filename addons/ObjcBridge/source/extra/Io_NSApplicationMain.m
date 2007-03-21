#include "Io_NSApplicationMain.h"
#include "IoState.h"
#include "IoObject.h"

IoObject *Io_NSApplicationMainRun(IoState *state)
{
    NSString *path = [[[NSBundle mainBundle] resourcePath] 
    stringByAppendingPathComponent:@"main.io"];
    IoState_doFile_(state, (char *)[path cString]);
    
    NSApplicationMain(state->mainArgs->argc, (const char **)state->mainArgs->argv);
    return 0x0;
}

void IoBindingsInit(IoState *self, IoObject *context); 

int Io_NSApplicationMain(int argc, const char *argv[])
{
    IoState *self = IoState_new();
    IoState_argc_argv_(self, argc, argv);
    IoState_setBindingsInitCallback(self, (IoStateBindingsInitCallback *)IoBindingsInit);
    IoState_init(self);

    Io_NSApplicationMainRun(self);
    
    return 0;
}



