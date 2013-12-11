//metadoc NullAddon copyright Samuel A. Falvo II, 2007
//metadoc NullAddon license BSD revised
//metadoc NullAddon category Server
/*metadoc NullAddon description
A do-nothing addon for Io, primarily intended to serve as
a skeleton for writing your own addons, without having to
rewrite all the boilerplate yourself. It implements only
one method, hello, which does the obvious job of greeting the world.
*/

#include "IoState.h"
#include "IoNumber.h"       // Needed?
#include "IoSeq.h"          // Needed?

#include "IoNullAddon.h"

/*
 * This macro makes it easier to access your object's internal data.
 */
#define DATA(self) ((IoNullAddonData *)IoObject_dataPointer(self))

static const char *protoId = "NullAddon";

/*
 * IoTag structures are type descriptors, which includes the linkages
 * necessary to:
 *
 * 1) Allow the interpreter to check the type of an object at run-time.
 * 2) Allow the interpreter to actually invoke your methods.
 * 3) other stuff which isn't quite so important at this level.
 */
IoTag *IoNullAddon_newTag(void *state)
{
    /* The tag name determines the type name. */
	IoTag *tag = IoTag_newWithName_(protoId);

    /* Since some functions don't accept an explicit IoState parameter,
     * we rely on back-links to provide access to the containing state.
     */
	IoTag_state_(tag, state);

    /* All types need to free their internal data, and ours is no
     * exception.
     */
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoNullAddon_free);

    /* And, likewise, all objects need to be able to be cloned.  For
     * simple objects, this could be as simple as a memcpy, but sometimes
     * greater intelligence is needed.
     */
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoNullAddon_rawClone);

    /* Add other IoTag initializations here, if you require them. */

	return tag;
}

/*
 * I'm not sure why this function is named xxx_proto, but its purpose
 * appears to be to instantiate a new Io object and initialize its
 * prototypes and initial slot tables.
 */
IoNullAddon *IoNullAddon_proto(void *state)
{
    /* Instantiate an object, as seen in the Io run-time environment,
     * not forgetting to also set its type.
     */
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoNullAddon_newTag(state));
	
    /* Instantiate _OUR_ object, and associate it with the Io object, so
     * we can access it through subsequent method calls.
     *
     * Normally, you'd initialize the object's internal state here as
     * well.  Since we don't really have any state (we're just allocating
     * memory for the sake of showing you how it's done), we'll skip this
     * step.
     */
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoNullAddonData)));
	IoState_registerProtoWithFunc_(state, self, IoNullAddon_proto);
	
    /* Associate our initial table of methods.  DO NOT include data or
     * variable here -- only methods (each of these will appear as
     * CFunctions inside the Io environment).  Note that we do this
     * inside a set of naked braces, so as to not pollute the function's
     * namespace unnecessarily.  This is not strictly required, of
     * course.
     *
     * Notice that the convention for method naming, while not set in
     * stone, typically follows the Smalltalk selector model.  Given a
     * function:
     *
     *   aFoo someInput: x with: y andWith: z
     *
     * You'd typically name the C function
     * IoFoo_someInput_with_andWith_().  However, this style is generally
     * relaxed when porting interfaces for other C libraries, since often
     * times, it's more important to conform to the library's API
     * standards than to Io's.
     *
     * We terminate the mapping of (method name, function pointer) pairs
     * with a single (NULL, NULL) mapping.
     */
	{
		IoMethodTable methodTable[] = {
		{"hello", IoNullAddon_hello},
		{NULL, NULL},
		};
		
		IoObject_addMethodTable_(self, methodTable);
	}

    /* At this point, the Io object is fully initialized, and is ready to
     * be returned to the interpreter.  Make sure that the C data is
     * suitably initialized as well!  Otherwise, you may crash the
     * interpreter when uninitialized fields are accessed.
     */
    return self;
}

/* Io is a prototype-based object environment, and the only way to
 * specialize objects from within Io is to clone them.  For normal Io
 * objects, this amounts to implementing differential inheritance, but
 * for C-provided bindings, this isn't always possible.  How does one
 * differentially inherit from a Curses interface, or from a GTK button?
 *
 * In these cases, Io relies on the domain-specific knowledge of the
 * addon to make the determination of how best to clone an object.
 * 
 * The simplest possible method is to just allocate a new structure, and
 * manually copy fields over.
 */
IoNullAddon *IoNullAddon_rawClone(IoNullAddon *proto)
{
    /* First, we need to instantiate a new Io object.  Most of the
     * initialization for this new object is to be taken from the
     * prototype, so we rely on an interpreter-provided function to take
     * care of all the boilerplate for us.
     */
	IoNullAddon *self = IoObject_rawClonePrimitive(proto);

    /* But we still need to copy over the library specific data. */
	IoObject_setDataPointer_(self, cpalloc(DATA(proto), sizeof(IoNullAddonData)));

    /* The object is now ready for use. */
	return self;
}

/* ----------------------------------------------------------- */

/* I am not at all sure why this function is needed, if the interpreter
 * can simply invoke IoNullAddon_proto() to get a new instance of an
 * object, or IoNullAddon_rawClone() to clone an existing one.  This
 * function is a total mystery to me.
 */
IoNullAddon *IoNullAddon_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

/* This function, however, is vitally important.  An object's state may
 * be garbage collected at any time, but the disposal of an object may
 * require special steps.  For example, before disposing of a File
 * object, you'll want to ensure that it is closed.
 *
 * This function is called just before the object is about to be
 * collected.  Notice that it returns 'void' -- disposal of an object
 * CANNOT fail.
 */
void IoNullAddon_free(IoNullAddon *self) 
{ 
	free(IoObject_dataPointer(self));
}

/* ----------------------------------------------------------- */

/* Here is our first method!! 
 * 
 * You'll notice that methods always take three parameters:
 *
 * self -- this is a pointer to the IoObject that is receiving the
 * message.
 *
 * locals -- this is the _context_ in which the method is executing.
 * If we need to reference another slot, it'll look here first.  This may
 * or may not be the same as self.  If you're familiar with the Message
 * doInContext() method, the argument to doInContext() appears here.
 *
 * m -- this is the message that was used to actually invoke this method.
 * This parameter is quite vital, because the message may require
 * arguments to be evaluated.  Or not, if you're implementing a macro.
 * :)
 */
IoObject *IoNullAddon_hello(IoNullAddon *self, IoObject *locals, IoMessage *m)
{ 
    printf("Hello world!\n");
	return self;
}

