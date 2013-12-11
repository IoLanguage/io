//metadoc NullAddon copyright Steve Dekorte and Edwin Zacharias, 2002
//metadoc NullAddon license BSD revised

#ifndef IONULLADDON_DEFINED
#define IONULLADDON_DEFINED 1

#include "IoObject.h"

/* Here, we create a convenient type synonym, so that our code is more
 * readable.  However, it's not a strict requirement.
 */
typedef IoObject IoNullAddon;

/* Our custom type will require some instance data.  Maybe.  If it does,
 * the convention is to tack "Data" onto the end of the type name.
 * Here, we're just going to throw some randomly named fields into a
 * structure.  We don't actually do anything with these in the NullAddon
 * because this is for demonstration purposes only.  Real, honest to
 * goodness addons will use this data though.
 */
typedef struct
{
    int foo, bar, baz;
    char *blort, blargh, **blech;
    float myBoat;
} IoNullAddonData;

/* Boilerplate functions required by the interpreter to play nice. */
IoNullAddon *IoNullAddon_proto(void *state);
IoNullAddon *IoNullAddon_rawClone(IoNullAddon *self);
IoNullAddon *IoNullAddon_new(void *state);
void IoNullAddon_free(IoNullAddon *self);

/* And here are our methods. */
IoObject *IoNullAddon_hello(IoObject *, IoObject *, IoObject *);

#endif
