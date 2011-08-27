#include "Runtime.h"

/*
#ifdef GNUSTEP
	unsigned int objc_getClassList(Class buffer[], int bufferLen)
	{
		Class class;
		void *iterator = NULL;
		unsigned int i = 0, classCount = 0;
		while ((class = objc_next_class(&iterator))) classCount++;
		iterator = NULL;
		while ((i < bufferLen) && (class = objc_next_class(&iterator))) buffer[i++]=class;
		return classCount;
	}
#else
	IMP __objc_get_forward_imp(SEL sel)
	{
		extern id _objc_msgForward(id self, SEL sel, ...);
		return _objc_msgForward;
	}
#endif
*/

//class_getMethodImplementation

/*
@implementation NSMethodSignature(Io2)
- (unsigned int)argumentSizeAtIndex:(unsigned int)index
{
#ifdef GNUSTEP
	return [self argumentInfoAtIndex:index].size;
#else
	return [self _argumentInfoAtIndex:index].size;
#endif
}
@end
*/


/**
 * This method creates a new ObjC class.
 * You can add methods to it using Io_objc_addMethod().
 * After adding methods the returned class must be registered with the runtime using
 * the runtime function objc_registerClassPair().
 */
Class Io_objc_makeClass(const char *name, const char *superName, BOOL isMetaClass)
{
	Class superClass = objc_getClass(superName);
    Class class = objc_allocateClassPair(superClass, name, 0);
    if (isMetaClass)
        class = object_getClass(class);
	return class;
}

void Io_class_addMethod(Class class, SEL sel, const char *types, IMP imp, BOOL toInstanceMethods)
{
	/*
	if (class == 0) return;
	if (toInstanceMethods == NO) class = class->isa;
	size_t size = sizeof(struct objc_method_list) + sizeof(struct objc_method[1]);
	struct objc_method_list *methodList = objc_calloc(1, size);
	if (methodList == 0) return;
#ifdef GNUSTEP
	sel = (SEL)GSNameFromSelector(sel);
#endif
	unsigned int num = (methodList->method_count)++;
	methodList->method_list[num].method_name = sel;
	methodList->method_list[num].method_types = strdup(types);
	methodList->method_list[num].method_imp = imp;
	extern void class_add_method_list(Class, struct objc_method_list *);
	extern void _objc_flush_caches(Class);
	class_addMethods(class, methodList);
	_objc_flush_caches(class);
	
	*/
	printf("ERROR Io_class_addMethod not supported\n");
	return;
}
