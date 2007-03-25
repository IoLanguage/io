/*#io
Duration ioDoc(
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IODURATION_DEFINED
#define IODURATION_DEFINED 1

#include <sys/types.h>
#include "Common.h"
#include "Duration.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

// extend message object

#define ISDURATION(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoDuration_rawClone)

typedef IoObject IoDuration;

IoDuration *IoMessage_locals_durationArgAt_(IoMessage *self, void *locals, int n);

IoDuration *IoDuration_proto(void *state);
IoDuration *IoDuration_rawClone(IoDuration *self);
IoDuration *IoDuration_new(void *state);
IoDuration *IoDuration_newWithSeconds_(void *state, double t);
void IoDuration_free(IoDuration *self);
int IoDuration_compare(IoDuration *self, IoDuration *other);
Duration *IoDuration_duration(IoDuration *self);

void IoDuration_writeToStream_(IoDuration *self, BStream *stream);
void IoDuration_readFromStream_(IoDuration *self, BStream *stream);

IoDuration *IoDuration_fromSeconds_(IoDuration *self, double s);
double IoDuration_asSeconds(IoDuration *self);

// getters/setters

IoObject *IoDuration_years(IoDuration *self, IoObject *locals, IoMessage *m);
IoObject *IoDuration_setYears(IoDuration *self, IoObject *locals, IoMessage *m);

IoObject *IoDuration_days(IoDuration *self, IoObject *locals, IoMessage *m);
IoObject *IoDuration_setDays(IoDuration *self, IoObject *locals, IoMessage *m);

IoObject *IoDuration_hours(IoDuration *self, IoObject *locals, IoMessage *m);
IoObject *IoDuration_setHours(IoDuration *self, IoObject *locals, IoMessage *m);

IoObject *IoDuration_minutes(IoDuration *self, IoObject *locals, IoMessage *m);
IoObject *IoDuration_setMinutes(IoDuration *self, IoObject *locals, IoMessage *m);

IoObject *IoDuration_seconds(IoDuration *self, IoObject *locals, IoMessage *m);
IoObject *IoDuration_setSeconds(IoDuration *self, IoObject *locals, IoMessage *m);
IoObject *IoDuration_totalSeconds(IoDuration *self, IoObject *locals, IoMessage *m);

IoObject *IoDuration_setFromTo(IoDuration *self, IoObject *locals, IoMessage *m);
IoObject *IoDuration_printDuration(IoDuration *self, IoObject *locals, IoMessage *m);

// conversion

IoObject *IoDuration_asString(IoDuration *self, IoObject *locals, IoMessage *m);
IoObject *IoDuration_asNumber(IoDuration *self, IoObject *locals, IoMessage *m);

IoObject *IoDuration_fromNumber(IoDuration *self, IoObject *locals, IoMessage *m);
IoObject *IoDuration_fromString(IoDuration *self, IoObject *locals, IoMessage *m);

// math

IoObject *IoDuration_add(IoDuration *self, IoObject *locals, IoMessage *m);
IoObject *IoDuration_subtract(IoDuration *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
