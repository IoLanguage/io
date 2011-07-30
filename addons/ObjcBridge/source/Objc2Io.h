/*   Copyright (c) 2003, Steve Dekorte
//metadoc license BSD revised
 *
 *  An Objective-C proxy to an Io value
 */

#ifndef __IO_OBJC2IO__
#define __IO_OBJC2IO__

#include "IoState.h"
#include "IoObject.h"
#include "IoObjcBridge.h"
#include "Runtime.h"

#define ISOBJC2IO(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)Objc2Io_rawClone)

@interface Objc2Io : NSObject // NSProxy
{
	IoObjcBridge *bridge;
	IoObject *ioValue;
}

+ withIoObject:(IoObject *)v;

- (void)setIoObject:(IoObject *)v;
- (IoObject *)ioValue;
- (void)setBridge:(IoObjcBridge *)b;

- (void)mark;

- (BOOL)respondsToSelector:(SEL)selector;
- (NSMethodSignature *)methodSignatureForSelector:(SEL)selector;
- (void)forwardInvocation:(NSInvocation *)invocation;

- (NSString *)description;

- (NSArray *)_rawSlotNames;
- (size_t)_rawSlotCount;
- (Objc2Io *)_rawGetSlot:(NSString *)slotName;
- (BOOL)_rawIsActivatable;


@end

#endif
