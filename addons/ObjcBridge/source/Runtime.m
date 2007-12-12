#include "Runtime.h"

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

Class objc_makeClass(const char *name, const char *superName, BOOL isMetaClass)
{
	Class superClass = objc_getClass(superName);
	Class class = objc_calloc(1, sizeof(struct objc_class));
	class->isa = (isMetaClass) ? superClass->isa->isa : objc_makeClass(name, superName, YES);
	class->name = (isMetaClass) ? strdup(name) : class->isa->name;
	class->info = (isMetaClass) ? CLS_META : CLS_CLASS;
	class->instance_size = (isMetaClass) ? superClass->isa->instance_size : superClass->instance_size;
#ifdef GNUSTEP
	class->super_class = (isMetaClass) ? (Class)strdup(superName) : class->isa->super_class;
#else
	class->super_class = (isMetaClass) ? superClass->isa : superClass;
	class->methodLists = objc_calloc(1, sizeof(struct objc_method_list *));
	class->methodLists[0] = (struct objc_method_list *)-1;
#endif
	return class;
}

void class_addMethod(Class class, SEL sel, const char *types, IMP imp, BOOL toInstanceMethods)
{
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
}
