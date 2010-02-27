/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoSampleRateConverter_DEFINED
#define IoSampleRateConverter_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include <samplerate.h>

#define ISSAMPLERATECONVERTER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSampleRateConverter_rawClone)

typedef IoObject IoSampleRateConverter;

typedef struct
{
	SRC_STATE *srcState;
	IoSeq *inputBuffer;
	IoSeq *outputBuffer;
	SRC_DATA *srcData;
} IoSampleRateConverterData;

IoSampleRateConverter *IoSampleRateConverter_proto(void *state);
IoSampleRateConverter *IoSampleRateConverter_new(void *state);
IoSampleRateConverter *IoSampleRateConverter_rawClone(IoSampleRateConverter *self);

void IoSampleRateConverter_mark(IoSampleRateConverter *self);
void IoSampleRateConverter_free(IoSampleRateConverter *self);

// -----------------------------------------------------------

IoObject *IoSampleRateConverter_start(IoSampleRateConverter *self, IoObject *locals, IoMessage *m);
IoObject *IoSampleRateConverter_process(IoSampleRateConverter *self, IoObject *locals, IoMessage *m);
IoObject *IoSampleRateConverter_stop(IoSampleRateConverter *self, IoObject *locals, IoMessage *m);

IoObject *IoSampleRateConverter_setOutputToInputRatio(IoSampleRateConverter *self, IoObject *locals, IoMessage *m);
IoObject *IoSampleRateConverter_outputToInputRatio(IoSampleRateConverter *self, IoObject *locals, IoMessage *m);
IoObject *IoSampleRateConverter_setEndOfInput(IoSampleRateConverter *self, IoObject *locals, IoMessage *m);

IoObject *IoSampleRateConverter_inputBuffer(IoSampleRateConverter *self, IoObject *locals, IoMessage *m);
IoObject *IoSampleRateConverter_outputBuffer(IoSampleRateConverter *self, IoObject *locals, IoMessage *m);

#endif
