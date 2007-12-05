
#import <Cocoa/Cocoa.h>
#import "IoState.h"
#import <objc/objc-runtime.h>

void IoDesktopInit(IoState *state);

@interface FooBar
{
}
- setInput:v;
- setOutput:v; 
@end

IoObject *main2(void *context, char *s);

int main(int argc, const char *argv[])
{
    IoState *self = IoState_new();
    IoState_pauseGarbageCollector(self);
    IoDesktopInit(self);
    
    IoObject_setSlot_to_(self->lobby, 
			 IoState_symbolWithCString_(self, "distribution"), 
			 IoState_symbolWithCString_(self, "IoDesktop"));
    
    IoState_resumeGarbageCollector(self);
    IoState_argc_argv_(self, argc, argv);
    
    {
	IoException *e;
	IoState_tryFunc_(self, (IoCatchCallback *)main2, self, 0x0, 0x0, &e);
    }
    return 0;
}

IoObject *main2(void *self, void *arg2, void *arg3)
{
    NSString *path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"../main.io"];
    IoState_mainRunFile_(self, (char *)[path cString]);

    {
	id c = [[objc_lookUpClass("Converter") alloc] init];
	id ti = [[[NSTextField alloc] init] retain];
	id to = [[[NSTextField alloc] init] retain];
	[ti setIntValue:1];
	//printf("s = %p\n", s);
	//[c respondsToSelector:@selector(setInput:)];
	//[c respondsToSelector:@selector(setOutput:)];
	//[c respondsToSelector:@selector(convert:)];
	[c setInput:ti];
	[c setOutput:to];
	[c convert:nil];
	printf("result = %i\n", [to intValue]);
    }
    return 0x0;
}


