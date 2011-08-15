//
//  IoLanguage.m
//  IoLanguageKit
//
//  Created by Stephen Dekorte on 20110720.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "IoLanguage.h"
#import "IoObjcBridge.h"
#import "IoNumber.h"
#import "Objc2Io.h"
#import "Io2Objc.h"

@protocol IoLanguageDelegate
- (void)printCallback:(NSString *)s;
- (void)exceptionCallback:anObject;
- (void)exitCallback:(int)r;
- (void)activeCoroCallback:(int)r;
@end


@implementation IoLanguage

//void IoAddonsInit(IoObject *context);
//IoState_setBindingsInitCallback(self, (IoStateBindingsInitCallback *)IoAddonsInit);

static IoLanguage *shared = nil;

+ (id)shared
{
	if (!shared) shared = [[IoLanguage alloc] init];
	return shared;
}

void IoSocketInit(IoObject *context);
void IoSystemCallInit(IoObject *context);
void IoBoxInit(IoObject *context);
void IoObjcBridgeInit(IoObject *context);


void IoLanguageStateBindingsInitCallback(void *context, void *state)
{
	IoSocketInit(state);
	IoSystemCallInit(state);
	IoBoxInit(state);
	IoObjcBridgeInit(state);
}

void IoLanguageStatePrintCallback(void *context, const UArray *array)
{
	id self = (id)context;
	NSString *out = [NSString stringWithUTF8String:(const char *)UArray_bytes(array)];
	if([self delegate] && [[self delegate] respondsToSelector:@selector(printCallback:)])
	{
		[[self delegate] printCallback:out];
	}
	else
	{
		printf("%s", [out UTF8String]);
	}
}

void IoLanguageStateExceptionCallback(void *context, IoObject *exceptionObject)
{
	id self = (id)context;
	if([self delegate] && [[self delegate] respondsToSelector:@selector(exceptionCallback:)])
	{
		[[self delegate] exceptionCallback:nil];
	}
	else
	{
		NSLog(@"io exception");
	}
}

void IoLanguageStateExitCallback(void *context, int r)
{
	id self = (id)context;
	if([self delegate] && [[self delegate] respondsToSelector:@selector(IoStateExitCallback:)])
	{
		[[self delegate] exitCallback:r];
	}
}

void ILanguageoStateActiveCoroCallback(void *context, int r)
{
	id self = (id)context;
	if([self delegate] && [[self delegate] respondsToSelector:@selector(IoStateExitCallback:)])
	{
		[[self delegate] activeCoroCallback:r];
	}
}

- init
{	
	state = IoState_new();
	//IoState_argc_argv_(self, argc, argv);
	IoState_callbackContext_(state, (void *)self);
	IoState_setBindingsInitCallback(state, IoLanguageStateBindingsInitCallback);
	IoState_printCallback_(state, IoLanguageStatePrintCallback);
	//IoState_activeCoroCallback_(state, ILanguageoStateActiveCoroCallback);
	IoState_exitCallback_(state, IoLanguageStateExitCallback);
	IoState_init(state);
	
	[self runIoResource:@"A0_EventManager"];
	[self runIoResource:@"A1_Socket"];
	[self runIoResource:@"A2_Server"];
	[self runIoResource:@"A3_Host"];
	[self runIoResource:@"A4_Object"];
	[self runIoResource:@"DNSResolver"];
	[self runIoResource:@"EvHttpCookie"];
	[self runIoResource:@"EvHttpServer"];
	[self runIoResource:@"EvOutResponse"];
	[self runIoResource:@"EvStatusCodes"];
	[self runIoResource:@"IPAddress"];
	[self runIoResource:@"URL"];
	[self runIoResource:@"UnixPath"];
	
	[self runIoResource:@"Vector"];
	[self runIoResource:@"ObjcBridge"];
	[self runIoResource:@"SystemCall"];
	return self;
}

- (void)dealloc
{
	IoState_free(state);
	[super dealloc];
}

- (void)setDelegate:anObject
{
	delegate = anObject;
}

- delegate
{
	return delegate;
}

- doString:(NSString *)s
{
	IoObject *result = IoState_doCString_(state, [s UTF8String]);
	
	if(ISSEQ(result)) // a string?
	{
		return [NSString stringWithUTF8String:(const char *)IoSeq_rawBytes(result)];
	}
	
	if (ISNUMBER(result))  // a number
	{
		return [NSNumber numberWithDouble:IoNumber_asDouble(result)];
	}
	
	if (ISIO2OBJC(result)) // a io proxy to objc? return the objc object 
	{
		return Io2Objc_object(result);
	}
	
	if(result) // unknown Io object? return a proxy to it
	{
		return [Objc2Io withIoObject:result];
	}
	
	return nil;
}

- (Objc2Io *)lobby
{
	return [Objc2Io withIoObject:IoState_lobby(state)];
}

- runIoResource:(NSString *)name
{
	NSString *path = [[NSBundle mainBundle] pathForResource:name ofType:@"io"];
	NSError *error;
	
	if(!path)
	{
		path = [[NSBundle bundleForClass:[IoLanguage class]] pathForResource:name ofType:@"io"];
	}
	
	if(path)
	{
		//NSLog(@"runIoResource:%@", path);
		NSString *code = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
		return [self doString:code];
	}
	
	return nil;
}

- runMain
{
	return [self runIoResource:@"main"];
}


@end
