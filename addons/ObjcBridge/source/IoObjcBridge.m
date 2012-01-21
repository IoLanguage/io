
//metadoc ObjcBridge copyright 2003, Steve Dekorte
//metadoc ObjcBridge license Revised BSD
//metadoc ObjcBridge category Bridges

#include "IoObjcBridge.h"
#include "List.h"
#include "IoState.h"
#include "IoList.h"
#include "IoMap.h"
#include "ObjcSubclass.h"
#include "IoBox.h"
#include "IoNumber.h"
#include "Io2Objc.h"
#include "Objc2Io.h"
#include "CHash_ObjcBridgeFunctions.h"

static const char *protoId = "ObjcBridge";

#define DATA(self) ((IoObjcBridgeData *)IoObject_dataPointer(self))

static IoObjcBridge *sharedBridge = NULL;

IoObjcBridge *IoObjcBridge_sharedBridge(void)
{
	return sharedBridge;
}

/*unsigned char IoObjcBridge_respondsTo(IoObject *self, IoObject *slotName) { return 1; }*/

List *IoObjcBridge_allClasses(IoObjcBridge *self)
{
	int n;

	if (DATA(self)->allClasses)
	{
		return DATA(self)->allClasses;
	}
	else
	{
		Class *classes = NULL;
		int numClasses = 0, newNumClasses = objc_getClassList(NULL, 0);

		DATA(self)->allClasses = List_new();

		while (numClasses < newNumClasses)
		{
			numClasses = newNumClasses;
			classes = objc_realloc(classes, sizeof(Class) * numClasses);
			newNumClasses = objc_getClassList(classes, numClasses);
		}

		for (n = 0; n < numClasses; n ++)
		{
			List_append_(DATA(self)->allClasses, classes[n]);
		}
		
		objc_free(classes); // memory leak - test
		return DATA(self)->allClasses;
	}
}

IoTag *IoObjcBridge_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoObjcBridge_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoObjcBridge_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoObjcBridge_mark);
	return tag;
}

IoObjcBridge *IoObjcBridge_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoObjcBridge_newTag(state));

	IoObject_setDataPointer_(self, objc_calloc(1, sizeof(IoObjcBridgeData)));

	DATA(self)->io2objcs = CHash_new();
    CHash_setObjcBridgeHashFunctions(DATA(self)->io2objcs);
	
	DATA(self)->objc2ios = CHash_new();
    CHash_setObjcBridgeHashFunctions(DATA(self)->objc2ios);
	
	IoObjcBridge_setMethodBuffer_(self, "nop");

	sharedBridge = self;

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
			{"classNamed", IoObjcBridge_classNamed},
			{"debugOn", IoObjcBridge_debugOn},
			{"debugOff", IoObjcBridge_debugOff},
			{"newClassWithNameAndProto", IoObjcBridge_newClassNamed_withProto_},
			{"autoLookupClassNamesOn", IoObjcBridge_autoLookupClassNamesOn},
			{"autoLookupClassNamesOff", IoObjcBridge_autoLookupClassNamesOff},
			// Extras
			//{"NSSelectorFromString", IoObjcBridge_NSSelectorFromString},
			//{"NSStringFromSelector", IoObjcBridge_NSStringFromSelector},
			{"main", IoObjcBridge_main},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoObjcBridge *IoObjcBridge_rawClone(IoObjcBridge *self)
{
	return self;
}

IoObjcBridge *IoObjcBridge_new(void *state)
{
	return sharedBridge;
	//return IoState_protoWithId_(state, protoId);
}

void IoObjcBridge_free(IoObjcBridge *self)
{
	//sharedBridge = NULL;
	
	CHASH_FOREACH(DATA(self)->objc2ios, k, v, [(id)v autorelease]);
	CHASH_FOREACH(DATA(self)->io2objcs, k, v, Io2Objc_nullObjcBridge(v));

	CHash_free(DATA(self)->io2objcs);
	CHash_free(DATA(self)->objc2ios);
	objc_free(DATA(self)->methodNameBuffer);
	objc_free(IoObject_dataPointer(self));
}

void IoObjcBridge_mark(IoObjcBridge *self)
{
	// mark Io2Objc objects
	//CHash_FOREACH(DATA(self)->io2objcs, k, v, IoObject_shouldMark(v));
	// mark io values referenced by Objc2Io objects
	CHASH_FOREACH(DATA(self)->objc2ios, k, v, [(id)v mark]);
	[ObjcSubclass mark]; // mark io protos for ObjcSubclasses
}

/* ----------------------------------------------------------------- */

BOOL IoObjcBridge_rawDebugOn(IoObjcBridge *self)
{
	return DATA(self)->debug;
}

IoObject *IoObjcBridge_autoLookupClassNamesOn(IoObjcBridge *self, IoObject *locals, IoMessage *m)
{
	//doc ObjcBridge2 autoLookupClassNamesOn Adds a forward method to the Lobby to call ObjcBridge classNamed(message name). Returns self.
	IoState_doCString_(IOSTATE, "Lobby forward := method(m := call message name; v := ObjcBridge classNamed(m); if(v, return v, Exception raise(\"Lookup error, slot '\" .. m ..\"' not found\")))");
	return self;
}

IoObject *IoObjcBridge_autoLookupClassNamesOff(IoObjcBridge *self, IoObject *locals, IoMessage *m)
{
	//doc ObjcBridge autoLookupClassNamesOn Removes the ObjC forward method from the Lobby. Returns self.
	IoState_doCString_(IOSTATE, "Lobby removeSlot(\"forward\")");
	return self;
}

IoObject *IoObjcBridge_debugOn(IoObjcBridge *self, IoObject *locals, IoMessage *m)
{
	//doc ObjcBridge debug On Turns debugging on. Returns self
	DATA(self)->debug = YES;
	return self;
}

IoObject *IoObjcBridge_debugOff(IoObjcBridge *self, IoObject *locals, IoMessage *m)
{
	//doc ObjcBridge debug On Turns debugging off. Returns self
	DATA(self)->debug = NO;
	return self;
}

/*IoObject *IoObjcBridge_NSSelectorFromString(IoObjcBridge *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *name = IoMessage_locals_symbolArgAt_(m, locals, 0);
	NSString *s = [NSString stringWithCString:CSTRING(name)];
	SEL sel = NSSelectorFromString(s);
	return IONUMBER((int)sel);
}

IoObject *IoObjcBridge_NSStringFromSelector(IoObjcBridge *self, IoObject *locals, IoMessage *m)
{
	int s = IoMessage_locals_intArgAt_(m, locals, 0);
	NSString *string = NSStringFromSelector((SEL)s);
	if (!string) return IONIL(self);
	return IOSYMBOL([string cString]);
}*/

IoObject *IoObjcBridge_main(IoObjcBridge *self, IoObject *locals, IoMessage *m)
{
	//doc ObjcBridge main Calls NSApplicationMain().
	int argc = 1;
	const char *argv[] = {CSTRING(IoState_doCString_(IOSTATE, "System launchPath"))};
	NSApplicationMain(argc, argv);
	return self;
}

IoObject *IoObjcBridge_classNamed(IoObjcBridge *self, IoObject *locals, IoMessage *m)
{
	//doc ObjcBridge classNamed(aSeq) Returns a proxy to the ObjC class with the specified name or nil if no match is found.
	IoSymbol *name = IoMessage_locals_symbolArgAt_(m, locals, 0);
	id obj = objc_lookUpClass(CSTRING(name));

	if (!obj)
	{
		return IONIL(self);
	}

	return IoObjcBridge_proxyForId_(self, obj);
}

void *IoObjcBridge_proxyForId_(IoObjcBridge *self, id obj)
{
	Io2Objc *v = CHash_at_(DATA(self)->io2objcs, obj);

	if (!v)
	{
		v = Io2Objc_new(IOSTATE);
		Io2Objc_setBridge(v, self);
		Io2Objc_setObject(v, obj);
		CHash_at_put_(DATA(self)->io2objcs, obj, v);
		//printf("io2objcs %i\n", (int)CHash_size(DATA(self)->io2objcs));
	}
	
	return v;
}

void *IoObjcBridge_proxyForIoObject_(IoObjcBridge *self, IoObject *v)
{
	Objc2Io *obj = CHash_at_(DATA(self)->objc2ios, v);
	
	if (!obj)
	{
		obj = [[[Objc2Io alloc] init] autorelease];
		[obj setBridge:self];
		[obj setIoObject:v];
		//CHash_at_put_(DATA(self)->objc2ios, IOREF(v), obj);
		IoObjcBridge_addValue_(self, v, obj);
		//printf("objc2ios %i\n", (int)CHash_size(DATA(self)->objc2ios));
	}
	
	return obj;
}
 
IoMessage *IoObjcBridge_ioMessageForNSInvocation_(IoObjcBridge *self, NSInvocation *invocation)
{
	//printf("IoObjcBridge_ioMessageForNSInvocation_\n");
	int index;
	BOOL debug = IoObjcBridge_rawDebugOn(self);
	NSMethodSignature *signature = [invocation methodSignature];
	char *methodName = IoObjcBridge_ioMethodFor_(self, (char *)sel_getName([invocation selector]));
	
	if(!strcmp(methodName, "sheetDidEnd:returnCode:contextInfo:"))
	{
		debug ++;
		signature = [NSMethodSignature signatureWithObjCTypes:"^@:iiii"];
		//signature = [NSMethodSignature signatureWithObjCTypes:"@:@q^v"];

		//methodName = "sheetDidEnd:returnCode:contextInfo:";
		//signature = [NSObject methodSignatureForSelector:NSSelectorFromString(@"my_sheetDidEnd:returnCode:contextInfo:")];
	}
	
	IoMessage *message = IoMessage_newWithName_(IOSTATE, IoState_symbolWithCString_(IOSTATE, methodName));
	const char *returnType = [[invocation methodSignature] methodReturnType];
	
	if (!*returnType) 
	{
		returnType = "?";
	}
	
	if (debug)
	{
		IoState_print_(IOSTATE, "Objc -> Io: ");
		printf("%s", IoObject_name([[invocation target] ioValue]));
		IoState_print_(IOSTATE, " (%s)", IoObjcBridge_nameForTypeChar_(self, *returnType));
		IoState_print_(IOSTATE, "%s(", methodName);
	}

	for (index = 2; index < [signature numberOfArguments]; index++)
	{
		char *error;
		const char *type = [signature getArgumentTypeAtIndex:index];
		//unsigned char buffer[[signature argumentSizeAtIndex:index]];
		unsigned char buffer[10];
		
		if (debug)
		{
			if (2 < index) printf(", ");
			printf("%s [%c]", IoObjcBridge_nameForTypeChar_(self, *type), *type);
		}
		[invocation getArgument:buffer atIndex:index];
		IoMessage_setCachedArg_to_(message, index-2, IoObjcBridge_ioValueForCValue_ofType_error_(self, buffer, (char *)type, &error));
		if (error)
			IoState_error_(IOSTATE, message, "Io IoObjcBridge ioMessageForNSInvocation %s - argtype:'%s' argnum:%i", error, type, index-2);
	}
	
	if (debug) 
	{
		printf(")\n");
	}
	
	if(!strcmp(methodName, "sheetDidEnd:returnCode:contextInfo:"))
	{
		debug --;
	}
	
	return message;
}

void IoObjcBridge_removeId_(IoObjcBridge *self, id obj)
{
	/* Called by Io2Objc instance when freed */
	CHash_removeKey_(DATA(self)->io2objcs, obj);
}

void IoObjcBridge_removeValue_(IoObjcBridge *self, IoObject *v)
{
	/* Called by Objc2Io instance when dealloced */
	CHash_removeKey_(DATA(self)->objc2ios, v);
}

void IoObjcBridge_addValue_(IoObjcBridge *self, IoObject *v, id obj)
{
	/* Called by Objc2Io instance when alloced */
	[obj retain];
	CHash_at_put_(DATA(self)->objc2ios, IOREF(v), obj);
}

const char *IoObjcBridge_selectorEncoding(IoObjcBridge *self, SEL selector)
{
	struct objc_method_description description;
	
	//description = [@protocol(AddressBook) descriptionForInstanceMethod:selector];
	description = protocol_getMethodDescription(@protocol(AddressBook), selector, YES, YES);
	if (description.name) return description.types;
	
	//description = [@protocol(AppKit) descriptionForInstanceMethod:selector];
	description = protocol_getMethodDescription(@protocol(AppKit), selector, YES, YES);
	if (description.name) return description.types;

//	description = [@protocol(Foundation) descriptionForInstanceMethod:selector];
	description = protocol_getMethodDescription(@protocol(Foundation), selector, YES, YES);
	if (description.name) return description.types;

	List *classes = IoObjcBridge_allClasses(self);
	size_t i, max = List_size(classes);
	
	for (i = 0; i < max; i++)
	{
		Class class = List_at_(classes, i);
		//if(i == 145) NSLog(@"className 145: %@", NSStringFromClass(class));
		Method method = class_getInstanceMethod(class, selector);
		if (!method) method = class_getClassMethod(class, selector);
		if (method)
			return method_getTypeEncoding(method);
	}
	return NULL;
}

// -----------------------------------------------------------------
//  Objective-C  -> Io
// -----------------------------------------------------------------

IoObject *IoObjcBridge_ioValueForCValue_ofType_error_(IoObjcBridge *self, void *cValue, const char *cType, char **error)
{
    #warning IoObjcBridge_ioValueForCValue_ofType_error_ doesn't check for cycles
	*error = NULL;
	//printf("cType: %s\n", cType);
	switch (*cType)
	{
		case '@':
		{
			id object = *(id *)cValue;
			
			if (!object)
			{
				return IONIL(self);
			}
			else if ([object isKindOfClass:[Objc2Io class]])
			{
				return [object ioValue];
			}
			else if ([object isKindOfClass:[NSString class]])
			{
				return IOSYMBOL([object UTF8String]);
			}
			else if ([object isKindOfClass:[NSNumber class]])
			{
				return IONUMBER([object doubleValue]);
			}
			else if ([object isKindOfClass:[NSArray class]])
			{
				IoList *ioList = IoList_new(IOSTATE);
				
				for(id v in (NSArray *)object)
				{
                    IoObject *ioValue = IoObjcBridge_ioValueForCValue_ofType_error_(self, &v, "@", error);
                    if (*error)
                    {
                        return IONIL(self);
                    }
                    else
                    {
                        IoList_rawAppend_(ioList, ioValue);
                    }
				}
				return ioList;
			}
            else if ([object isKindOfClass:[NSDictionary class]])
			{
				IoMap *ioMap = IoMap_new(IOSTATE);
				for(id k in [(NSDictionary *)object allKeys])
				{
                    id v = [(NSDictionary *)object objectForKey:k];
                    
                    IoObject *ioValue;
                    
                    if ([k isKindOfClass:[NSString class]])
                    {
                        ioValue = IoObjcBridge_ioValueForCValue_ofType_error_(self, &v, "@", error);
                        if (*error)
                        {
                            return IONIL(self);
                        }
                    }
                    else
                    {
                        ioValue = IoObjcBridge_proxyForId_(self, v);
                    }
                    
                    IoMap_rawAtPut(ioMap, IOSYMBOL([(NSString *)k UTF8String]), ioValue);
				}
                return ioMap;
			}
			else
			{
				return IoObjcBridge_proxyForId_(self, object);
			}
		}
		case '#':
		{
			Class class = *(Class *)cValue;
			
			if (!class)
			{
				return IONIL(self);
			}
			else
			{
				return IoObjcBridge_proxyForId_(self, class);
			}
		}
		case ':':
		{
			SEL selector = *(SEL *)cValue;
			
			if (selector)
			{
				return IOSYMBOL(sel_getName(selector)); 
			}
			else
			{
				*error = "null selector";
			}
			break;
		}
		case 'c': return IoNumber_newWithDouble_(IOSTATE, *(char *)cValue);
		case 'i': return IoNumber_newWithDouble_(IOSTATE, *(int *)cValue);
		case 's': return IoNumber_newWithDouble_(IOSTATE, *(short *)cValue);
		case 'l': return IoNumber_newWithDouble_(IOSTATE, *(long *)cValue);
		case 'q': return IoNumber_newWithDouble_(IOSTATE, *(long long *)cValue);
		case 'C': return IoNumber_newWithDouble_(IOSTATE, *(unsigned char *)cValue);
		case 'I': return IoNumber_newWithDouble_(IOSTATE, *(unsigned int *)cValue);
		case 'S': return IoNumber_newWithDouble_(IOSTATE, *(unsigned short *)cValue);
		case 'L': return IoNumber_newWithDouble_(IOSTATE, *(unsigned long *)cValue);
		case 'Q': return IoNumber_newWithDouble_(IOSTATE, *(unsigned long long *)cValue);
		case 'f': return IoNumber_newWithDouble_(IOSTATE, *(float *)cValue);
		case 'd': return IoNumber_newWithDouble_(IOSTATE, *(double *)cValue);
		case 'B': return IoNumber_newWithDouble_(IOSTATE, *(int *)cValue);  // C++ bool
		//case 'v': return IONIL(IOSTATE);  // void
		case '*': return IoState_symbolWithCString_(IOSTATE, *(char **)cValue);
			//case ':': return IoState_symbolWithCString_(IOSTATE, (SEL *)cValue); 
		//case '[': an array
				
		case '{':
			if (!strncmp(cType, "{CGPoint=dd}", 12))
			{
				CGPoint p = *(CGPoint *)cValue;
				vec2f v;
				v.x = p.x;
				v.y = p.y;
				return IoSeq_newVec2f(IOSTATE, v);
			}
			if (!strncmp(cType, "{_NSPoint=ff}", 13))
			{
				NSPoint p = *(NSPoint *)cValue;
				vec2f v;
				v.x = p.x;
				v.y = p.y;
				return IoSeq_newVec2f(IOSTATE, v);
			}
			else if (!strncmp(cType, "{_NSSize=ff}", 12))
			{
				NSSize s = *(NSSize *)cValue;
				vec2f v;
				v.x = s.width;
				v.y = s.height;
				return IoSeq_newVec2f(IOSTATE, v);
			}
			else if (!strncmp(cType, "{CGSize=dd}", 11))
			{
				CGSize s = *(CGSize *)cValue;
				vec2f v;
				v.x = s.width;
				v.y = s.height;
				return IoSeq_newVec2f(IOSTATE, v);
			}
			else if (!strncmp(cType, "{_NSRect={_NSPoint=ff}{_NSSize=ff}}", 35))
			{
				NSRect r = *(NSRect *)cValue;
				return IoBox_newSet(IOSTATE, r.origin.x, r.origin.y, 0, r.size.width, r.size.height, 0);
			}
			else if (!strncmp(cType, "{CGRect={CGPoint=dd}{CGSize=dd}}", 32))
			{
				NSRect r = *(NSRect *)cValue;
				return IoBox_newSet(IOSTATE, r.origin.x, r.origin.y, 0, r.size.width, r.size.height, 0);
			}
			else
			{
				*error = "unsupported struct";
			}
			break;
		case '^':
			if (!strncmp(cType, "^v", 2))
			{
				return IONIL(self);
				// do we assume it's an Io object? how do we test for that?
				//IoObject *iobj = *(IoObject **)cValue;
				//if (!iobj) return IONIL(self);
				//return iobj;
			}
			else
			{
				*error = "unsupported pointer type";
			}
			break;
			
		default:
			*error = "no match for argument type";
	}
	
	return IONIL(self);
}

// -----------------------------------------------------------------
//  Io -> Objective-C
// -----------------------------------------------------------------

void *IoObjcBridge_cValueForIoObject_ofType_error_(IoObjcBridge *self, IoObject *value, const char *cType, char **error)
{
	*error = NULL;
	
	switch (*cType)
	{
		case '@':
			if (ISMUTABLESEQ(value))
				DATA(self)->cValue.o = [NSMutableString stringWithUTF8String:UTF8CSTRING(value)];
			else if (ISSYMBOL(value))
				DATA(self)->cValue.o = [NSString stringWithUTF8String:UTF8CSTRING(value)];
			else if (ISNUMBER(value))
				DATA(self)->cValue.o = [NSNumber numberWithInt:IoNumber_asInt(value)];
			else if (ISIO2OBJC(value))
				DATA(self)->cValue.o = Io2Objc_object(value);
			else if (ISNIL(value))
				DATA(self)->cValue.o = nil;
			else if (ISLIST(value))
			{
				char *error;
				size_t i, count = IoList_rawSize(value);
				id objects[count];
				for (i = 0; i < count; i ++)
					objects[i] = *(id *)IoObjcBridge_cValueForIoObject_ofType_error_(self, IoList_rawAt_(value, (int)i), "@", &error);
				DATA(self)->cValue.o = [NSArray arrayWithObjects:objects count:count];
			}
			else if (ISMAP(value))
			{
				char *error;
				IoList *list = IoMap_rawKeys(value);
				size_t i, count = IoList_rawSize(list);
				id keys[count], objects[count];
				for (i = 0; i < count; i ++)
				{
					keys[i] = *(id *)IoObjcBridge_cValueForIoObject_ofType_error_(self, IoList_rawAt_(list, (int)i), "@", &error);
					objects[i] = *(id *)IoObjcBridge_cValueForIoObject_ofType_error_(self, IoMap_rawAt(value, IoList_rawAt_(list, (int)i)), "@", &error);
				}
				DATA(self)->cValue.o = [NSDictionary dictionaryWithObjects:objects forKeys:keys count:count];
			}
			else
				DATA(self)->cValue.o = IoObjcBridge_proxyForIoObject_(self, value);
			break;
		case '#':
			if (ISIO2OBJC(value))
				DATA(self)->cValue.class = Io2Objc_object(value);
			else
				DATA(self)->cValue.class = nil;
			break;
		case ':':
			if (ISSYMBOL(value))
				DATA(self)->cValue.sel = sel_getUid(CSTRING(value));
			else
				*error = "requires a string";
			break;
		case 'c':case 'C':
			if (ISNUMBER(value))
				DATA(self)->cValue.c = IoNumber_asInt(value);
			else if (ISBOOL(value))
				DATA(self)->cValue.c = ISTRUE(value);
			else
				*error = "requires a number or a boolean";
			break;
		case 's':case 'S':
			if (ISNUMBER(value))
				DATA(self)->cValue.s = IoNumber_asInt(value);
			else
				*error = "requires a number";
			break;
		case 'i':case 'I':
			if (ISNUMBER(value))
				DATA(self)->cValue.i = IoNumber_asInt(value);
			else
				*error = "requires a number";
			break;
		case 'Q':case 'q':
			if (ISNUMBER(value))
				DATA(self)->cValue.LL = IoNumber_asLong(value);
			else
				*error = "requires a number";
			break;
		case 'l':case 'L':
			if (ISNUMBER(value))
				DATA(self)->cValue.l = IoNumber_asLong(value);
			else
				*error = "requires a number";
			break;
		case 'f':
			if (ISNUMBER(value))
				DATA(self)->cValue.f = IoNumber_asDouble(value);
			else
				*error = "requires a number";
			break;
		case 'd':
			if (ISNUMBER(value))
				DATA(self)->cValue.d = IoNumber_asDouble(value);
			else
				*error = "requires a number";
			break;
		//case 'b':
		//case 'v':
		case 'r': // const qualifier
			if (0 != strncmp(cType, "r*", 2)) // strings do not equal
			{
				*error = "no match for argument type";
				break;
			}
			// fall through to '*' if strings are equal
		case '*':
			if (ISSYMBOL(value))
			{
				DATA(self)->cValue.cp = CSTRING(value);
			}
			else
			{
				*error = "requires a string";
				break;
			}
		case '^':
			if (!strncmp(cType, "^@", 2))
			{
				if (ISIO2OBJC(value))
				{
					DATA(self)->cValue.v = &((Io2ObjcData *)IoObject_dataPointer(value))->object;
				}
				else
				{
					*error = "requires an Io2Objc";
				}
			}
			else if (!strncmp(cType, "^v", 2))
			{
				if (ISSYMBOL(value))
				{
					DATA(self)->cValue.v = CSTRING(value);
				}
				else if(ISNIL(value))
				{
					DATA(self)->cValue.v = nil;
				}
				else
				{
					//DATA(self)->cValue.v = value; //not safe
					DATA(self)->cValue.v = (void *)0x0;
					//*error = "requires a string";
				}
			}
			else
			{
				*error = "no match for argument type";
			}
			break;
		case '{':
			if (!strcmp(cType, "{_NSPoint=ff}"))
			{
				if (ISVECTOR(value))
				{
					vec2f v = IoSeq_vec2f(value);
					DATA(self)->cValue.point.x = v.x;
					DATA(self)->cValue.point.y = v.y;
				}
				else
				{
					*error = "requires a Point";
				}
			}
			else if (!strcmp(cType, "{CGPoint=dd}"))
			{
				if (ISVECTOR(value))
				{
					vec2f v = IoSeq_vec2f(value);
					DATA(self)->cValue.point.x = v.x;
					DATA(self)->cValue.point.y = v.y;
				}
				else
				{
					*error = "requires a Point";
				}
			}
			else if (!strcmp(cType, "{_NSSize=ff}"))
			{
				if (ISVECTOR(value))
				{
					vec2f v = IoSeq_vec2f(value);
					DATA(self)->cValue.size.width  = v.x;
					DATA(self)->cValue.size.height = v.y;
				}
				else
				{
					*error = "requires a Point";
				}
			}
			else if (!strcmp(cType, "{CGSize=dd}"))
			{
				if (ISVECTOR(value))
				{
					vec2f v = IoSeq_vec2f(value);
					DATA(self)->cValue.size.width  = v.x;
					DATA(self)->cValue.size.height = v.y;
				}
				else
				{
					*error = "requires a Point";
				}
			}
			else if (!strcmp(cType, "{_NSRect={_NSPoint=ff}{_NSSize=ff}}"))
			{
				if (ISBOX(value))
				{
					vec2f v1 = IoSeq_vec2f(IoBox_rawOrigin(value));
					vec2f v2 = IoSeq_vec2f(IoBox_rawSize(value));
					
					DATA(self)->cValue.rect.origin.x = v1.x;
					DATA(self)->cValue.rect.origin.y = v1.y;
					
					DATA(self)->cValue.rect.size.width  = v2.x;
					DATA(self)->cValue.rect.size.height = v2.y;
				}
				else
				{
					*error = "requires a Box containing 2 points";
				}
			}
			else if (!strcmp(cType, "{CGRect={CGPoint=dd}{CGSize=dd}}"))
			{
				if (ISBOX(value))
				{
					vec2f v1 = IoSeq_vec2f(IoBox_rawOrigin(value));
					vec2f v2 = IoSeq_vec2f(IoBox_rawSize(value));
					
					DATA(self)->cValue.rect.origin.x = v1.x;
					DATA(self)->cValue.rect.origin.y = v1.y;
					
					DATA(self)->cValue.rect.size.width  = v2.x;
					DATA(self)->cValue.rect.size.height = v2.y;
				}
				else
				{
					*error = "requires a Box containing 2 points";
				}
			}
			else
			{
				*error = "no match for argument type";
			}
			break;
		default:
			*error = "no match for argument type";
	}
	
	return &DATA(self)->cValue;
}

/* --- method name buffer ----------------------------------- */

void IoObjcBridge_setMethodBuffer_(IoObjcBridge *self, char *name)
{
	size_t length = strlen(name);
	
	if (length > DATA(self)->methodNameBufferSize)
	{
		DATA(self)->methodNameBuffer = objc_realloc(DATA(self)->methodNameBuffer, length+1);
		DATA(self)->methodNameBufferSize = (int)length;
	}
	
	strcpy(DATA(self)->methodNameBuffer, name);
}

char *IoObjcBridge_ioMethodFor_(IoObjcBridge *self, char *name)
{
	/*IoObjcBridge_setMethodBuffer_(self, name);
	{
		char *s = DATA(self)->methodNameBuffer;
		while (*s) { if (*s == ':') {*s = '_';} s++; }
	}
	return DATA(self)->methodNameBuffer;*/
	return name;
}

char *IoObjcBridge_objcMethodFor_(IoObjcBridge *self, char *name)
{
	/*IoObjcBridge_setMethodBuffer_(self, name);
	{
		char *s = DATA(self)->methodNameBuffer;
		while (*s) { if (*s == '_') {*s = ':';} s++; }
	}
	return DATA(self)->methodNameBuffer;*/
	return name;
}

/* --- new classes -------------------------------------------- */

IoObject *IoObjcBridge_newClassNamed_withProto_(IoObjcBridge *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *ioSubClassName = IoMessage_locals_symbolArgAt_(m, locals, 0);
	IoObject *proto = IoMessage_locals_valueArgAt_(m, locals, 1);
	char *subClassName = CSTRING(ioSubClassName);
	Class sub = objc_lookUpClass(subClassName);

	if (sub)
	{
		IoState_error_(IOSTATE, m, "Io ObjcBridge newClassNamed_withProto_ '%s' class already exists", subClassName);
	}
	
	sub = [ObjcSubclass newClassNamed:ioSubClassName proto:proto];
	return IoObjcBridge_proxyForId_(self, sub);
}

char *IoObjcBridge_nameForTypeChar_(IoObjcBridge *self, char type)
{
	switch (type)
	{
		case '@': return "id";
		case '#': return "Class";
		case ':': return "SEL";
		case 'c': return "char";
		case 'C': return "unsigned char";
		case 's': return "short";
		case 'S': return "unsigned short";
		case 'i': return "int";
		case 'I': return "unsigned int";
		case 'l': return "long";
		case 'L': return "unsigned long";
		case 'q': return "long long";
		case 'f': return "float";
		case 'd': return "double";
		case 'b': return "bitfield";
		case 'v': return "void";
		case '?': return "undefined";
		case '^': return "pointer";
		case '*': return "char *";
		case '[': return "array B";
		case ']': return "array E";
		case '(': return "union B";
		case ')': return "union E";
		case '{': return "struct B";
		case '}': return "struct A";
		case 'r': // const qualifier
			if (strncmp(&type, "r*", 2))
				return "const char *";
	}
	return "?";
}
