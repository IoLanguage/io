#ifndef __IO_OBJC_RUNTIME__
#define __IO_OBJC_RUNTIME__

/*
 * This is a set of macros that translate Apple Objective-C runtime call to
 * GNU Objective-C runtime call.
 */

#ifdef GNUSTEP
	#include <AppKit/AppKit.h>
	#undef sel_getUid
	#define isa class_pointer
	#define CLS_META _CLS_META
	#define CLS_CLASS _CLS_CLASS
	#define sel_getUid sel_get_uid
	#define objc_addClass(class) GSObjCAddClasses([NSArray arrayWithObject:[NSValue valueWithPointer:(class)]])
	unsigned int objc_getClassList(Class buffer[], int bufferLen);
	extern IMP __objc_get_forward_imp(SEL sel);
#else
	#import <AppKit/AppKit.h>
	#import <objc/Protocol.h>
	#import <objc/objc-runtime.h>
	#define objc_free free
	#define objc_calloc calloc
	#define objc_malloc malloc
	#define objc_realloc realloc
	#define sel_get_type(selector) (NULL)
	#define sel_get_typed_uid(name, type) sel_getUid((name))
	#define sel_register_typed_name(name, type) sel_registerName((name))
	IMP __objc_get_forward_imp(SEL sel);
	struct _arginfo
	{
		unsigned int offset;
		unsigned int size;
		const char *type;
		struct _arginfo *tmp1;
		unsigned int tmp2;
	};
	@interface NSMethodSignature(Io)
	+ (NSMethodSignature *)signatureWithObjCTypes:(const char *)encoding;
	- (struct _arginfo)_argumentInfoAtIndex:(unsigned int)index;
	@end
#endif
@interface NSMethodSignature(Io2)
- (unsigned int)argumentSizeAtIndex:(unsigned int)index;
@end
Class objc_makeClass(const char *name, const char *superName, BOOL isMetaClass);
void class_addMethod(Class class, SEL sel, const char *types, IMP imp, BOOL toInstanceMethods);

#endif
