//metadoc Random copyright Steve Dekorte 2002
//metadoc Random license BSD revised
/*metadoc Random description
A high quality and reasonably fast random number generator based on 
Makoto Matsumoto, Takuji Nishimura, and Eric Landry's implementation 
of the <a href=http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html>Mersenne Twister</a> algorithm. The default seed is a xor of 
the ANSI C time() and clock() return values.
*/
//metadoc Random category Math

#include "IoRandom.h"
#include "IoNumber.h"
#include "RandomGen.h"
#include "BStream.h"

#define DATA(self) ((RandomGen *)(IoObject_dataPointer(self)))

static const char *protoId = "Random";

void IoRandom_writeToStream_(IoRandom *self, BStream *stream)
{
	RandomGen *r = DATA(self);
	int i;

	for (i = 0; i < RANDOMGEN_N; i ++)
	{
		BStream_writeTaggedUint32_(stream, r->mt[i]);
	}

	BStream_writeTaggedUint32_(stream, r->mti);
}

void *IoRandom_readFromStream(IoRandom *self, BStream *stream)
{
	RandomGen *r = DATA(self);
	int i;

	for (i = 0; i < RANDOMGEN_N; i ++)
	{
		r->mt[i] = BStream_readTaggedUint32(stream);
	}

	r->mti = BStream_readTaggedUint32(stream);
	return self;
}

IoTag *IoRandom_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoRandom_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoRandom_free);
	//IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc *)IoRandom_writeToStream);
	//IoTag_readFromStreamFunc_(tag, (IoTagReadFromStreamFunc *)IoRandom_readFromStream);
	return tag;
}

IoRandom *IoRandom_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"value", IoRandom_value},
	{"setSeed", IoRandom_setSeed},
	{"flip", IoRandom_flip},
	{"gaussian", IoRandom_gaussian},
	{"bytes", IoRandom_bytes},
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoRandom_newTag(state));
	IoObject_setDataPointer_(self, RandomGen_new());

	RandomGen_chooseRandomSeed(DATA(self));

	IoState_registerProtoWithId_((IoState *)state, self, protoId);

	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoNumber *IoRandom_rawClone(IoRandom *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, RandomGen_new());
	RandomGen_chooseRandomSeed(DATA(self));
	return self;
}

void IoRandom_free(IoMessage *self)
{
	RandomGen_free(DATA(self));
}

IoObject *IoRandom_flip(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Random flip
	Returns a random bit as a true or false object.
	*/
	int r = RandomGen_randomInt(DATA(self));
	return IOBOOL(self, r & 0x1);
}

IoObject *IoRandom_value(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Random value(optionalArg1, optionalArg2)
	If called with:
	<ul>
	<li> no arguments, it returns a floating point
	random Number between 0 and 1.
	<li> one argument, it returns a floating point random
	Number between 0 and optionalArg1.
	<li> two arguments, it returns a floating point random
	Number between optionalArg1 and optionalArg2.
	</ul>
	*/

	double f = RandomGen_randomDouble(DATA(self));
	double result = 0;

	if (IoMessage_argCount(m) > 0)
	{
		double a = IoMessage_locals_doubleArgAt_(m, locals, 0);

		if (IoMessage_argCount(m) > 1)
		{
			double b = IoMessage_locals_doubleArgAt_(m, locals, 1);

			if (a == b )
			{
				result = a;
			}
			else
			{
				result = a + (b - a) * f;
			}
		}
		else
		{
			if (a == 0)
			{
				result = 0;
			}
			else
			{
				result = a * f;
			}
		}
	}
	else
	{
		result = f;
	}

	return IONUMBER(result);
}

IoObject *IoRandom_setSeed(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Random setSeed(aNumber)
	Sets the random number generator seed to the unsigned int version of aNumber.
	*/

	unsigned long v = IoMessage_locals_longArgAt_(m, locals, 0);
	RandomGen_setSeed(DATA(self), v);
	return self;
}

IoObject *IoRandom_gaussian(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Random gaussian(optionalMean, optionalStandardDeviation)
	Returns a pseudo random number between 0 and 1 with a gaussian distribution.
	*/

	double mean = 0;
	double standardDeviation = 1;

	if (IoMessage_argCount(m) > 0)
	{
		mean = IoMessage_locals_doubleArgAt_(m, locals, 0);
	}

	if (IoMessage_argCount(m) > 1)
	{
		standardDeviation = IoMessage_locals_doubleArgAt_(m, locals, 1);
	}

	return IONUMBER(RandomGen_gaussian(DATA(self), mean, standardDeviation));
}

IoObject *IoRandom_bytes(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Random bytes(count)
	Returns a Sequence of size count containing random bytes.
	*/

	size_t i, count = IoMessage_locals_sizetArgAt_(m, locals, 0);
	UArray *a;
	uint8_t *d = malloc(count);

	for(i = 0; i < count; i ++)
	{
		d[i] = (uint8_t)(RandomGen_randomInt(DATA(self)) & 255);
	}

	a = UArray_newWithData_type_size_copy_(d, CTYPE_uint8_t, count, 0);
	UArray_setEncoding_(a, CENCODING_NUMBER);

	return IoSeq_newWithUArray_copy_(IOSTATE, a, 0);
}
