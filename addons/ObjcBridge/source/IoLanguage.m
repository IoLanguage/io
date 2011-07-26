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

void IoObjcBridgeInit(IoObject *context);

void IoLanguageStateBindingsInitCallback(void *context, void *state)
{
	IoObjcBridgeInit(state);
}

void IoLanguageStatePrintCallback(void *context, const UArray *array)
{
	id self = (id)context;
	if([self delegate] && [[self delegate] respondsToSelector:@selector(printCallback:)])
	{
		[[self delegate] printCallback:[NSString stringWithUTF8String:(const char *)UArray_bytes(array)]];
	}
}

void IoLanguageStateExceptionCallback(void *context, IoObject *ioObj)
{
	id self = (id)context;
	if([self delegate] && [[self delegate] respondsToSelector:@selector(exceptionCallback:)])
	{
		[[self delegate] exceptionCallback:nil];
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
		Objc2Io *objc2io = [[[Objc2Io alloc] init] autorelease];
		[objc2io setIoObject:result];
		[objc2io setBridge:IoObjcBridge_sharedBridge()];
		return objc2io;
	}
	
	return nil;
}

@end
