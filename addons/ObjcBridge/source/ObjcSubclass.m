/*   Copyright (c) 2003, Steve Dekorte
*   All rights reserved. See _BSDLicense.txt.
*/

#include "ObjcSubclass.h"
#include "List.h"
#include "IoState.h"
#include "IoMessage.h"
#include "Objc2Io.h"
#include "CHash_ObjcBridgeFunctions.h"
#include <stdlib.h>

/*
 * globals are evil, but Objective-C classes are globals already,
 * so who cares if we have these globals for tracking the ones we add?
 *
 * IMPORTANT: can't use this addon with multiple Io states.
 */

static IoState *state = NULL;
static CHash *classProtos = NULL;

@implementation ObjcSubclass

+ (id)new
{
	return [[self alloc] init];
}

+ (CHash *)classProtos
{
	if (!classProtos)
    {
        classProtos = CHash_new();
        CHash_setObjcBridgeHashFunctions(classProtos);
    }
	return classProtos;
}

+ (void)mark
{
	if (classProtos)
		CHASH_FOREACH(classProtos, k, v, IoObject_shouldMark(v));
}

+ (Class)newClassNamed:(IoSymbol *)ioName proto:(IoObject *)proto
{
	Class class = Io_objc_makeClass(CSTRING(ioName), "ObjcSubclass", NO);
	objc_registerClassPair(class);
	state = IoObject_state(proto);
	if (class)
        CHash_at_put_([self classProtos], ioName, proto);
	return class;
}

/*- (BOOL)respondsToSelector:(SEL)sel
{
	BOOL r = [super respondsToSelector:sel];
	//printf("ObjcSubclass respondsToSelector:\"%s\"] = %i\n", (char *)sel_getName(sel), r);
	return r;
}*/

+ (id)allocWithZone:(NSZone *)zone
{
	id v = [super allocWithZone:zone];
	//printf("ObjcSubclass allocWithZone\n");
	[v setProto];
	return v;
}

+ (id)alloc
{
	id v = [super alloc];
	//printf("[ObjcSubclass alloc]\n");
	//[v setProto];
	return v;
}

- (void)setProto
{
	const char *s = [[self className] UTF8String];

	if(IoObjcBridge_rawDebugOn(IoObjcBridge_sharedBridge()))
		printf("classname = %s state = %p\n", s, state);

	if (state)
	{
		IoSymbol *className = IoState_symbolWithCString_(state, (char *)s);
		IoObject *proto = (IoObject *)CHash_at_((void *)[[self class] classProtos], className);
		[super init];
		[self setIoObject: (IoObject *)IOCLONE(proto)];
		[self setBridge: IoObjcBridge_sharedBridge()];
		IoObjcBridge_addValue_(bridge, ioValue, self);
	}
}

- copy
{
	id obj = [super copy];
	NSMethodSignature *methodSignature=[self methodSignatureForSelector:@selector(copy)];
	NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:methodSignature];
	[invocation setTarget:obj];
	[invocation setSelector:@selector(copy)];
	[self forwardInvocation:invocation];
	[invocation getReturnValue:&obj];
	return obj;
}

//- copyWithZone:(NSZone *)zone
//{
//	id obj = [super copyWithZone:zone];
//	NSMethodSignature *methodSignature = [self methodSignatureForSelector:@selector(copyWithZone:)];
//	NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:methodSignature];
//	[invocation setTarget:obj];
//	[invocation setSelector:@selector(copyWithZone:)];
//	[invocation setArgument:zone atIndex:2];
//	[self forwardInvocation:invocation];
//	[invocation getReturnValue:&obj];
//	return obj;
//}

- init
{
	id obj = [super init];
	NSMethodSignature *methodSignature = [self methodSignatureForSelector:@selector(init)];
	NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:methodSignature];
	[invocation setTarget:obj];
	[invocation setSelector:@selector(init)];
	[self forwardInvocation:invocation];
	[invocation getReturnValue:&obj];
	return obj;
}

- mutableCopy
{
	id obj = [super mutableCopy];
	NSMethodSignature *methodSignature = [self methodSignatureForSelector:@selector(mutableCopy)];
	NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:methodSignature];
	[invocation setTarget:obj];
	[invocation setSelector:@selector(mutableCopy)];
	[self forwardInvocation:invocation];
	[invocation getReturnValue:&obj];
	return obj;
}

@end
