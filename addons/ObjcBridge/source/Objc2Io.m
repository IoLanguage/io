/*   Copyright (c) 2003, Steve Dekorte
*   All rights reserved. See _BSDLicense.txt.
*/

#include "Objc2Io.h"
#include "List.h"

@implementation Objc2Io

- init
{
	self = [super init];
	//[obj retain]; // debug test
	return self;
}

- (void)dealloc
{
	//if (IoObjcBridge_rawDebugOn(bridge)) IoState_print_(IoObject_state(bridge), "[Objc2Io %p dealloc]\n", self);
	IoObjcBridge_removeValue_(bridge, ioValue);
	[super dealloc];
}

- (void)setIoObject:(IoObject *)v
{
	ioValue = v;

	IoTag* tag = IoObject_tag(ioValue);
	IoTag_performFunc_(tag, (IoTagPerformFunc *)IoObject_perform);
	
	IoObjcBridge_addValue_(bridge, ioValue, self);
}

- (IoObject *)ioValue
{
	return ioValue;
}

- (void)setBridge:(IoObjcBridge *)b
{
	bridge = b;
}

- (void)mark
{
	if (bridge) IoObject_shouldMark((IoObject *)bridge);
	if (ioValue) IoObject_shouldMark((IoObject *)ioValue);
}

- (BOOL)respondsToSelector:(SEL)selector
{
	IoState *state = IoObject_state(bridge);
	BOOL debug = IoObjcBridge_rawDebugOn(bridge);
	char *ioMethodName = IoObjcBridge_ioMethodFor_(bridge, (char *)sel_getName(selector));

	if (debug)
		IoState_print_(state, "[Objc2Io respondsToSelector:\"%s\"] ", ioMethodName);

	IoSymbol *methodName = IoState_symbolWithCString_(state, ioMethodName);
	BOOL result = IoObject_rawGetSlot_((IoObject *)ioValue, methodName) ? YES : NO;

	if (debug)
		IoState_print_(state, "= %i\n", result);

	return result;
}

/*- (void)doesNotRecognizeSelector:(SEL)aSelector
{
	printf("-------------------------- Objc2Io doesNotRecognizeSelector:\\n");
}*/

- (NSMethodSignature *)methodSignatureForSelector:(SEL)selector
{
	{
		const char *encoding = IoObjcBridge_selectorEncoding(bridge, selector);
		if (encoding)
			return [NSMethodSignature signatureWithObjCTypes:encoding];
	}

	// Note: some methods are dynamically generated, e.g. setter/getters by InterfaceBuilder
	// they are in the form setVariable:, ecc...
	// the following code provide a generic signature of the form @:@@@@
	// the result will be an id and all the arguments are id

	{
		unsigned int argCount = 0;
		const char *name = sel_getName(selector);
		while (*name) argCount += (*name++ == ':');
		char *encoding = objc_malloc((argCount + 4) * sizeof(char));
		memset(encoding, '@', argCount + 3);
		encoding[argCount + 3] = 0;
		encoding[2] = ':';
		NSMethodSignature *signature = [NSMethodSignature signatureWithObjCTypes:encoding];
		objc_free(encoding);
		return signature;
	}
}

- (void)forwardInvocation:(NSInvocation *)invocation
{

	// perform io message
	
	IoMessage *message = IoObjcBridge_ioMessageForNSInvocation_(bridge, invocation);
	const char *returnType = [[invocation methodSignature] methodReturnType];
	IoTag* tag = IoObject_tag(ioValue);
	if (!IoTag_performFunc(tag))
		IoState_error_(IoObject_state(ioValue), message, "Io Objc2Io forwardInvocation: no performFunc set on object: %s", IoObject_name(ioValue));
	IoObject *result = tag->performFunc(ioValue, ioValue, message);

	// convert and return result if not void

	if (*returnType != 'v')
	{
		char *error;
		void *cResult = IoObjcBridge_cValueForIoObject_ofType_error_(bridge, result, returnType, &error);
		if (error)
			IoState_error_(IoObject_state(bridge), message, "Io Objc2Io forwardInvocation: %s - return type:'%s'", error, returnType);
		[invocation setReturnValue:cResult];
	}
}

@end
