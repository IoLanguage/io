
//metadoc Number copyright Steve Dekorte 2002
//metadoc Number license BSD revised
/*metadoc Number description
A container for a double (a 64bit floating point number on most platforms).
*/
//metadoc Number category Core

#define _GNU_SOURCE // for round
#include "IoNumber.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoSeq.h"
#include "IoDate.h"
#include "IoState.h"
#include <math.h>
#include <ctype.h>
#include <assert.h>

#include <setjmp.h>
#if defined(_BSD_PPC_SETJMP_H_)
#include <machine/limits.h>
#else
#include <limits.h>
#endif

#if defined(__SYMBIAN32__)
/* TODO: Fix symbian constants */
#define FLT_MAX 0.0
#define FLT_MIN 0.0
#else
#include <float.h>
#endif

#ifdef _MSC_VER
#define isnan _isnan
#endif

#define DATA(self) CNUMBER(self)

IoNumber *IoNumber_numberForDouble_canUse_(IoNumber *self, double n, IoNumber *other)
{
	if (DATA(self)  == n) return self;
	if (DATA(other) == n) return other;
	return IONUMBER(n);
}

IoTag *IoNumber_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Number");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoNumber_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoNumber_free);
	IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoNumber_compare);
	IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc *)IoNumber_writeToStream_);
	IoTag_readFromStreamFunc_(tag, (IoTagReadFromStreamFunc *)IoNumber_readFromStream_);
	assert(sizeof(double) <= sizeof(void *)*2);
	/*printf("Number tag = %p\n", (void *)tag);*/
	return tag;
}

void IoNumber_writeToStream_(IoNumber *self, BStream *stream)
{
	BStream_writeTaggedDouble_(stream, DATA(self));
}

void *IoNumber_readFromStream_(IoNumber *self, BStream *stream)
{
	DATA(self) = BStream_readTaggedDouble(stream);
	return self;
}

// #define IONUMBER_IS_MUTABLE

IoNumber *IoNumber_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"asNumber", IoNumber_asNumber},
	{"+", IoNumber_add_},
	{"-", IoNumber_subtract},
	{"*", IoNumber_multiply},
	{"/", IoNumber_divide},
	//{"print", IoNumber_printNumber},

	{"asString", IoNumber_asString},
	{"asBuffer", IoNumber_asBuffer},
	{"asCharacter", IoNumber_asCharacter},
	{"asUint32Buffer", IoNumber_asUint32Buffer},
	//{"asDate", IoNumber_asDate},

	{"abs", IoNumber_abs},
	{"acos", IoNumber_acos},
	{"asin", IoNumber_asin},
	{"atan", IoNumber_atan},
	{"atan2", IoNumber_atan2},
	{"ceil", IoNumber_ceil},
	{"cos", IoNumber_cos},
	// {"deg", IoNumber_deg}
	{"exp", IoNumber_exp},
	{"factorial", IoNumber_factorial},
	{"floor", IoNumber_floor},
	{"log", IoNumber_log},
	{"log2", IoNumber_log2},
	{"log10", IoNumber_log10},
	{"max", IoNumber_max},
	{"min", IoNumber_min},
	{"%", IoNumber_mod},
	{"mod", IoNumber_mod},
	{"**", IoNumber_pow},
	{"pow", IoNumber_pow},
	{"round", IoNumber_round},
	{"roundDown", IoNumber_roundDown},
	{"sin", IoNumber_sin},
	{"sqrt", IoNumber_sqrt},
	{"squared", IoNumber_squared},
	{"cubed", IoNumber_cubed},
	{"tan", IoNumber_tan},
	{"toggle", IoNumber_toggle},

	// logic operations

	{"&", IoNumber_bitwiseAnd},
	{"|", IoNumber_bitwiseOr},
	{"^", IoNumber_bitwiseXor},
	{"<<", IoNumber_bitShiftLeft},
	{">>", IoNumber_bitShiftRight},

	{"bitwiseAnd", IoNumber_bitwiseAnd},
	{"bitwiseOr", IoNumber_bitwiseOr},
	{"bitwiseXor", IoNumber_bitwiseXor},
	{"bitwiseComplement", IoNumber_bitwiseComplement},
	{"shiftLeft", IoNumber_bitShiftLeft},
	{"shiftRight", IoNumber_bitShiftRight},

	// even and odd

	{"isEven", IoNumber_isEven},
	{"isOdd", IoNumber_isOdd},

	// character operations

	{"isAlphaNumeric", IoNumber_isAlphaNumeric},
	{"isLetter", IoNumber_isLetter},
	{"isControlCharacter", IoNumber_isControlCharacter},
	{"isDigit", IoNumber_isDigit},
	{"isGraph", IoNumber_isGraph},
	{"isLowercase", IoNumber_isLowercase},
	{"isUppercase", IoNumber_isUppercase},
	{"isPrint", IoNumber_isPrint},
	{"isPunctuation", IoNumber_isPunctuation},
	{"isSpace", IoNumber_isSpace},
	{"isHexDigit", IoNumber_isHexDigit},

	{"asLowercase", IoNumber_asLowercase},
	{"asUppercase", IoNumber_asUppercase},

	{"between", IoNumber_between},
	{"clip", IoNumber_clip},
	{"negate", IoNumber_negate},
	{"at", IoNumber_at},

	{"integerMax", IoNumber_integerMax},
	{"integerMin", IoNumber_integerMin},
	{"longMax", IoNumber_longMax},
	{"longMin", IoNumber_longMin},
	{"shortMax", IoNumber_shortMax},
	{"shortMin", IoNumber_shortMin},
	{"unsignedLongMax", IoNumber_unsignedLongMax},
	{"unsignedIntMax", IoNumber_unsignedIntMax},
	{"floatMax", IoNumber_floatMax},
	{"floatMin", IoNumber_floatMin},
	{"isNan", IoNumber_isNan},

	{"repeat", IoNumber_repeat},

	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoNumber_newTag(state));
	DATA(self) = 0;
	IoState_registerProtoWithFunc_((IoState *)state, self, IoNumber_proto);

	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoNumber *IoNumber_rawClone(IoNumber *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	DATA(self) = DATA(proto);
	return self;
}

IoNumber *IoNumber_newWithDouble_(void *state, double n)
{
	IoNumber *proto = IoState_protoWithInitFunction_((IoState *)state, IoNumber_proto);
	IoNumber *self = IOCLONE(proto); // since Numbers have no refs, we can avoid IOCLONE
	DATA(self) = n;
	return self;
}

IoNumber *IoNumber_newCopyOf_(IoNumber *self)
{
	return IONUMBER(DATA(self));
}

void IoNumber_copyFrom_(IoNumber *self, IoNumber *number)
{
	DATA(self) = DATA(number);
}

void IoNumber_free(IoNumber *self)
{
	/* need this so Object won't try to io_free IoObject_dataPointer(self) */
}

UArray IoNumber_asStackUArray(IoNumber *self)
{
	UArray a = UArray_stackAllocedEmptyUArray();
	a.size = 1;

	if (sizeof(double) == 4)
	{
		a.itemType = CTYPE_float32_t;
		a.itemSize = 4;
	}
	else
	{
		a.itemType = CTYPE_float64_t;
		a.itemSize = 8;
	}

	a.data = (uint8_t *)(&DATA(self));
	return a;
}

int IoNumber_asInt(IoNumber *self)
{
	return (int)(DATA(self));
}

long IoNumber_asLong(IoNumber *self)
{
	return (long)(DATA(self));
}

float IoNumber_asFloat(IoNumber *self)
{
	return (float)DATA(self);
}

double IoNumber_asDouble(IoNumber *self)
{
	return (double)DATA(self);
}

int IoNumber_compare(IoNumber *self, IoNumber *v)
{
	if (ISNUMBER(v))
	{
		if (DATA(self) == DATA(v))
		{
			return 0;
		}
		return (DATA(self) > DATA(v)) ? 1 : -1;
	}
	return IoObject_defaultCompare(self, v);
}

void IoNumber_Double_intoCString_(double n, char *s, size_t maxSize)
{
	if (n == (int)n)
	{
		snprintf(s, maxSize, "%d", (int)n);
	}
	else if (n > INT_MAX)
	{
		snprintf(s, maxSize, "%e", n);
	}
	else
	{
		int l;

		snprintf(s, maxSize, "%.16f", n);

		// remove the trailing zeros ex: 10.00 -> 10

		l = strlen(s) - 1;

		while (l > 0)
		{
			if (s[l] == '0') { s[l] = 0; l--; continue; }
			if (s[l] == '.') { s[l] = 0; l--; break; }
			break;
		}
	}
}

void IoNumber_print(IoNumber *self)
{
	double d = DATA(self);
	char s[128];

	IoNumber_Double_intoCString_(d, s, 127);
	IoState_print_(IOSTATE, "%s", s);
}

// -----------------------------------------------------------

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

//IoObject *IoNumber_htonl(IoNumber *self, IoObject *locals, IoMessage *m)
//{
//    /* doc htonl
//    Returns a new number with the first 4 bytes of the receiver switched from
//host to network byte order.
//    */
//
//    IoNumber *num = IONUMBER(0);
//    IoObject_setDataUint32_(num, htonl(IoObject_dataUint32(self)));
//    return num;
//}

//IoObject *IoNumber_ntohl(IoNumber *self, IoObject *locals, IoMessage *m)
//{
//    /* doc ntohl
//Returns a new number with the first 4 bytes of the receiver switched from
//network to host byte order.
//    */
//
//	IoNumber *num = IONUMBER(0);
//	IoObject_setDataUint32_(num, ntohl(IoObject_dataUint32(self)));
//	return num;
//}

// -----------------------------------------------------------

IoObject *IoNumber_asNumber(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number asNumber
	Returns self.
	*/
	
	return self;
}

IoObject *IoNumber_add_(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number +(aNumber)
	Returns a new number that is the sum of the receiver and aNumber.
	*/
	
	IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
	return IONUMBER(DATA(self) + DATA(other));
}


IoObject *IoNumber_subtract(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number -(aNumber)
	Returns a new number that is the difference of the receiver and aNumber.
	*/
	
	IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
	return IONUMBER(DATA(self) - DATA(other));
}

IoObject *IoNumber_divide(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number /(aNumber)
	Returns a new number with the value of the receiver diveded by aNumber.
	*/
	
	IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
	return IONUMBER(DATA(self) / DATA(other));
}

IoObject *IoNumber_multiply(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number *(aNumber)
	Returns a new number that is the product of the receiver and aNumber.
	*/
	
	IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
	return IONUMBER(DATA(self) * DATA(other));
}

char *IoNumber_asAllocedCString(IoNumber *self)
{
	IoSymbol *string;
	int size = 1024;
	char *s = (char *)io_calloc(1, size);
	memset(s, 0, size);
	IoNumber_Double_intoCString_(DATA(self), s, size - 1);
	return s;
}

IoObject *IoNumber_printNumber(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number print
	Prints the number.
	*/
	
	char *s = IoNumber_asAllocedCString(self);
	IoState_print_((IoState *)IOSTATE, s);
	io_free(s);
	return self;
}

IoObject *IoNumber_justAsString(IoNumber *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *string;
	char *s = IoNumber_asAllocedCString(self);
	string = IoSeq_newWithCString_((IoState *)IOSTATE, s);
	io_free(s);
	return string;
}

IoObject *IoNumber_asCharacter(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number asCharacter
	Returns a String containing a single character whose
	value is the ascii value of the first byte of the receiver.
	*/
	
	char s[2];
	s[0] = (char)DATA(self);
	s[1] = 0;
	return IoState_symbolWithCString_length_((IoState *)IOSTATE, s, 1);
}

IoObject *IoNumber_asUint32Buffer(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number asUint32Buffer
	Returns a Sequence containing a 4 byte representation of the uint32 value of the receiver.
	*/
	
	uint32_t i = (int)DATA(self);
	return IoSeq_newWithData_length_(IOSTATE, (unsigned char *)&i, sizeof(uint32_t));
}

IoObject *IoNumber_asBuffer(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number asBuffer(optionalNumberOfBytes)
	Returns a Buffer containing a the number of bytes specified by
	optionalNumberOfBytes (up to the size of a double on the platform) of the reciever.
	If no optionalNumberOfBytes is specified, it is assumed to be the number of bytes
	in a double on the host platform.
	*/
	IoNumber *byteCount = IoMessage_locals_valueArgAt_(m, locals, 0);
	int bc = sizeof(double);

	if (!ISNIL(byteCount))
	{
		bc = DATA(byteCount);
	}
	return IoSeq_newWithData_length_(IOSTATE, (unsigned char *)&(DATA(self)), bc);
}

IoObject *IoNumber_asString(IoNumber *self, IoObject *locals, IoMessage *m)
{
/*doc Number asString(optionalIntegerDigits, optionalFactionDigits)
Returns a string representation of the receiver. For example:
<pre>
1234.5678 asString(0, 2)
</pre>	
would return:
<pre>
$1234.56
</pre>	
*/
	
	if (IoMessage_argCount(m) >= 1)
	{
		int whole = IoMessage_locals_intArgAt_(m, locals, 0);
		int part = 6;
		char *s;
		size_t length;
		IoObject *n;


		if (IoMessage_argCount(m) >= 2)
		{
			part = abs(IoMessage_locals_intArgAt_(m, locals, 1));
		}

		part  = abs(part);
		whole = abs(whole);

		// If whole == 0, printf might need an arbitary size string. Instead of
		// second guessing the size, pick a really big size: 1024.
		length = 1024;
		s = io_calloc(1, length);

		snprintf(s, length, "%*.*f", whole, part, DATA(self));

		n = IOSEQ((unsigned char *)s, (size_t)strlen(s));

		io_free(s);

		return n;
	}

	return IoNumber_justAsString(self, locals, m);
}

/*
IoObject *IoNumber_asDate(IoNumber *self, IoObject *locals, IoMessage *m)
{
	return IoDate_newWithNumber_((IoState *)IOSTATE, DATA(self));
}
*/

IoObject *IoNumber_abs(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number abs
	Returns a number with the absolute value of the receiver.
	*/
	
	return (DATA(self) < 0) ? (IoObject *)IONUMBER(-DATA(self)) : (IoObject *)self;
}

IoObject *IoNumber_acos(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number acos
	Returns a number with the arc cosine of the receiver.
	*/
	
	return IONUMBER(acos(DATA(self)));
}

IoObject *IoNumber_asin(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number asin
	Returns a number with the arc sine of the receiver.
	*/
	
	return IONUMBER(asin(DATA(self)));
}

IoObject *IoNumber_atan(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number atan
	Returns a number with the arc tangent of the receiver.
	*/
	
	return IONUMBER(atan(DATA(self)));
}

IoObject *IoNumber_atan2(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number atan2(aNumber)
	Returns a number with the arc tangent of y/x where y is the receiver and x is aNumber.
	*/
	
	IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
	return IONUMBER(atan2(DATA(self), DATA(other)));
}

IoObject *IoNumber_ceil(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number ceil
	Returns the a number with the receiver's value rounded up to
	the nearest integer if it's fractional component is greater than 0.
	*/
	
	return IONUMBER(ceil(DATA(self)));
}

IoObject *IoNumber_cos(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number cos
	Returns the cosine of the receiver.
	*/
	
	return IONUMBER(cos(DATA(self)));
}

/*
IoObject *IoNumber_deg(IoNumber *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(deg(DATA(self)));
}
*/

IoObject *IoNumber_exp(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number exp
	Returns e to the power of the receiver.
	*/
	
	return IONUMBER(exp(DATA(self)));
}

IoObject *IoNumber_factorial(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number factorial
	Returns the factorial of the receiver.
	*/
	
	int n = DATA(self);
	double v = 1;
	while (n)
	{
		v *= n;
		n--;
	}
	return IONUMBER(v);
}

IoObject *IoNumber_floor(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number floor
	Returns the a number with the receiver's value rounded
	down to the nearest integer if it's fractional component is not 0.
	*/
	
	return IONUMBER(floor(DATA(self)));
}

IoObject *IoNumber_log(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number log
	Returns the logarithm of the receiver.  The base
	is taken as the value of the first argument or the constant e if
	the first argument is omitted.
	*/

	float base;
	if(IoMessage_argCount(m) > 0){
		base = DATA(IoMessage_locals_numberArgAt_(m, locals, 0));
	}
	else{
		base = M_E;
	}
	return IONUMBER(log(DATA(self)) / log(base));
}

IoObject *IoNumber_log2(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number log2
	Returns the base 2 logarithm of the receiver.
	*/

	return IONUMBER(log(DATA(self)) / log(2));
}

IoObject *IoNumber_log10(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number log10
	Returns the base 10 logarithm of the receiver.
	*/

	return IONUMBER(log10(DATA(self)));
}

IoObject *IoNumber_max(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number max(aNumber)
	Returns the greater of the receiver and aNumber.
	*/

	IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
	return (DATA(self) > DATA(other)) ? (IoObject *)self :(IoObject *)other;
}

IoObject *IoNumber_min(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number min(aNumber)
	Returns the lesser of the receiver and aNumber.
	*/

	IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
	return (DATA(self) < DATA(other)) ? (IoObject *)self : (IoObject *)other;
}

IoObject *IoNumber_mod(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number %(aNumber)
	Returns the receiver modulus aNumber.
	*/

	/*doc Number mod(aNumber)
	Returns the receiver modulus aNumber.
	*/

	IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
	return IONUMBER(fmod(DATA(self), DATA(other)));
}

/*
IoObject *IoNumber_modf(IoNumber *self, IoObject *locals, IoMessage *m)
{
	IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
	if (DATA(self) < DATA(other)); return self;
	return other;
}

IoObject *IoNumber_rad(IoNumber *self, IoObject *locals, IoMessage *m)
*/

IoObject *IoNumber_pow(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number pow(aNumber)
	Returns the value of the receiver to the aNumber power.
	*/
	
	/*doc Number **(aNumber)
	Same as pow(aNumber).
	*/
	
	IoNumber *other = IoMessage_locals_numberArgAt_(m, locals, 0);
	return IONUMBER(pow(DATA(self), DATA(other)));
}

IoObject *IoNumber_round(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number round
	Returns the a number with the receiver's value rounded up to
	the nearest integer if it's fraction component is >= .5.
	*/
	
	double x = DATA(self);
	if (x < 0.0)
		return IONUMBER(ceil(x - 0.5));
	else
		return IONUMBER(floor(x + 0.5));
}

IoObject *IoNumber_roundDown(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number roundDown
	Returns the a number with the receiver's value rounded down to
	the nearest integer if it's fraction component is <= .5.
	*/
	
	return IONUMBER(floor(DATA(self) + 0.5));
}

IoObject *IoNumber_sin(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number sin
	Returns the sine of the receiver.
	*/

	return IONUMBER(sin(DATA(self)));
}

IoObject *IoNumber_sqrt(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number sqrt
	Returns the square root of the receiver.
	*/

	return IONUMBER(sqrt(DATA(self)));
}

IoObject *IoNumber_squared(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number squared
	Returns the square of the receiver.
	*/

	double v = DATA(self);
	return IONUMBER(v * v);
}

IoObject *IoNumber_cubed(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number cubed
	Returns the cube of the receiver.
	*/

	double v = DATA(self);
	return IONUMBER(v * v * v);
}


IoObject *IoNumber_tan(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number tan
	Returns the tangent of the receiver.
	*/

	return IONUMBER(tan(DATA(self)));
}

/*
IoObject *IoNumber_frexp(IoNumber *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER( frexp(DATA(self)) );
}

IoObject *IoNumber_ldexp(IoNumber *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER( ldexp(DATA(self)) );
}
*/

IoObject *IoNumber_toggle(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number toggle
	Returns 1 if the receiver is 0. Returns 0 otherwise.
	*/

	return (DATA(self))? (IoObject *)IONUMBER(0) : (IoObject *)IONUMBER(1);
}

/* --- bitwise operations ---------------------------------------- */

IoObject *IoNumber_bitwiseAnd(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number &(aNumber)
	Returns a new number with the bitwise AND of the receiver and aNumber.
	*/

	/*doc Number bitwiseAnd(aNumber)
	Returns a new number with the bitwise AND of the receiver and aNumber.
	*/

	long other = IoMessage_locals_longArgAt_(m, locals, 0);
	return IONUMBER(((long)DATA(self) & other));
}

IoObject *IoNumber_bitwiseOr(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number |(aNumber)
	Returns a new number with the bitwise OR of the receiver and aNumber.
	*/

	/*doc Number bitwiseOr(aNumber)
	Returns a new number with the bitwise AND of the receiver and aNumber.
	*/

	long other = IoMessage_locals_longArgAt_(m, locals, 0);
	long n = DATA(self);
	long r = n | other;
	return IONUMBER(r);
}

IoObject *IoNumber_bitwiseXor(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number bitwiseXor(aNumber)
	Returns a new number with the bitwise XOR of the receiver and aNumber.
	*/

	/*doc Number ^(aNumber)
	Returns the bitwise xor with the receiver (both numbers are converted to longs for the operation).
	*/

	long other = IoMessage_locals_longArgAt_(m, locals, 0);
	long r = (double)((long)DATA(self) ^ other);
	return IONUMBER(r);
}

IoObject *IoNumber_bitwiseComplement(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number bitwiseComplement
	Returns a new number with the bitwise complement of the
	receiver. (Turns the 0 bits of become 1s and the 1 bits become 0s. )
	*/

	long r = (double)(~(long)DATA(self));
	return IONUMBER(r);
}

IoObject *IoNumber_bitShiftLeft(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number shiftLeft(aNumber)
	Shifts the bits of the receiver left by the number of places specified by aNumber.
	*/

	long other = IoMessage_locals_longArgAt_(m, locals, 0);
	long r = (double)((long)DATA(self) << other);
	return IONUMBER(r);
}

IoObject *IoNumber_bitShiftRight(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number shiftRight(aNumber)
	Shifts the bits of the receiver right by the number of places specified by aNumber.
	*/

	long other = IoMessage_locals_longArgAt_(m, locals, 0);
	long r =  (double)((long)DATA(self) >> (long)other);
	return IONUMBER(r);
}

// even and odd ------------------------------

IoObject *IoNumber_isEven(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isEven
	Returns self if integer form of the receiver is even. Otherwise returns Nil.
	*/

	int n = DATA(self);
	return IOBOOL(self, 0 == (n & 0x01));
}

IoObject *IoNumber_isOdd(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isOdd
	Returns self if integer form of the receiver is even. Otherwise returns Nil.
	*/

	int n = DATA(self);
	return IOBOOL(self, 0x01 == (n & 0x01));
}

// character operations ---------------------------------

IoObject *IoNumber_isAlphaNumeric(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isAlphaNumeric
	Returns self if the receiver is an alphanumeric
	character value. Otherwise returns Nil.
	*/

	return IOBOOL(self, isalnum((int)DATA(self)));
}

IoObject *IoNumber_isLetter(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isLetter
	Returns self if the receiver is an alphanetic character value. Otherwise returns Nil.
	*/

	return IOBOOL(self, isalpha((int)DATA(self)));
}

IoObject *IoNumber_isControlCharacter(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isControlCharacter
	Returns self if the receiver is an control
	character value. Otherwise returns Nil.
	*/

	return IOBOOL(self, iscntrl((int)DATA(self)));
}

IoObject *IoNumber_isDigit(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isDigit
	Returns self if the receiver is an numeric
	digit character value. Otherwise returns Nil.
	*/

	return IOBOOL(self, isdigit((int)DATA(self)));
}

IoObject *IoNumber_isGraph(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isGraph
	Returns self if the receiver is a printing character
	value except space. Otherwise returns Nil.
	*/

	return IOBOOL(self, isgraph((int)DATA(self)));
}

IoObject *IoNumber_isLowercase(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isLowercase
	Returns self if the receiver is an lower case
	character value. Otherwise returns Nil.
	*/

	return IOBOOL(self, islower((int)DATA(self)));
}

IoObject *IoNumber_isUppercase(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isUppercase
	Returns self if the receiver is an upper case
	character value. Otherwise returns Nil.
	*/

	return IOBOOL(self, isupper((int)DATA(self)));
}

IoObject *IoNumber_isPrint(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isPrint
	Returns self if the receiver is an printing character
	value, including space. Otherwise returns Nil.
	*/

	return IOBOOL(self, isprint((int)DATA(self)));
}

IoObject *IoNumber_isPunctuation(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isPunctuation
	Returns self if the receiver is an printing character
	value, except space letter or digit. Otherwise returns Nil.
	*/

	return IOBOOL(self, ispunct((int)DATA(self)));
}

IoObject *IoNumber_isSpace(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isSpace
	Returns self if the receiver is a space, formfeed,
	newline carriage return, tab or vertical tab character value. Otherwise returns Nil.
	*/

	return IOBOOL(self, isspace((int)DATA(self)));
}

IoObject *IoNumber_isHexDigit(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isHexDigit
	Returns self if the receiver is hexidecimal digit
	character value. Otherwise returns Nil.
	*/

	return IOBOOL(self, isxdigit((int)DATA(self)));
}

// case ---------------------------------

IoObject *IoNumber_asLowercase(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number asLowercase
	Returns a new Number containing a lower case version of the receiver.
	*/

	int r = tolower((int)DATA(self));
	return IONUMBER(r);
}

IoObject *IoNumber_asUppercase(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number asUppercase
	Returns a new Number containing a upper case version of the receiver.
	*/

	int r = toupper((int)DATA(self));
	return IONUMBER(r);
}

IoObject *IoNumber_between(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number between(aNumber1, aNumber2)
	Returns the true if the receiver's value is between or
	equal to aNumber1 and aNumber2, otherwise returns false.
	*/

	double a = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double b = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double n = DATA(self);

	return IOBOOL(self, ((n >= a) && (n <= b)) || (n <= a && (n >= b)));
}

IoObject *IoNumber_clip(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number clip(aNumber1, aNumber2)
	Returns self if the receiver is between aNumber1 and aNumber2.
	Returns aNumber1 if it is less than aNumber1. Returns aNumber2 if it is greater than aNumber2.
	*/

	double a = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double b = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double n = DATA(self);

	if (n < a) n = a;
	if (n > b) n = b;

	return IONUMBER(n);
}

IoObject *IoNumber_negate(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number negate
	Returns new number that is negated version of the receiver.
	*/

	return IONUMBER(-DATA(self));
}

IoObject *IoNumber_at(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number at(bitIndexNumber)
	Returns a new Number containing 1 if the receiver cast to a long
	has it's bit set to 1 at bitIndexNumber. Otherwise returns 0.
	*/

	int i = IoMessage_locals_intArgAt_(m, locals, 0);
	long l = (long)DATA(self);

	IOASSERT((i >= 0) && (i < sizeof(double)*8), "index out of bit bounds");

	l = l >> i;
	l = l & 0x1;
	return IONUMBER(l);
}

// limits ------------------------------------

IoObject *IoNumber_integerMax(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number integerMax
	Returns the maximum integer value.
	*/

	return IONUMBER(INT_MAX);
}

IoObject *IoNumber_integerMin(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number integerMin
	Returns the minimum integer value.
	*/

	return IONUMBER(INT_MIN);
}


IoObject *IoNumber_longMax(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number longMax
	Returns the maximum long value.
	*/

	return IONUMBER(LONG_MAX);
}

IoObject *IoNumber_longMin(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number longMin
	Returns the minimum long value.
	*/

	return IONUMBER(LONG_MIN);
}


IoObject *IoNumber_shortMax(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number shortMax
	Returns the maximum short value.
	*/

	return IONUMBER(SHRT_MAX);
}

IoObject *IoNumber_shortMin(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number shortMin
	Returns the minimum short value.
	*/

	return IONUMBER(SHRT_MIN);
}

IoObject *IoNumber_unsignedLongMax(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number unsignedLongMax
	Returns the maximum unsigned long value.
	*/

	return IONUMBER(ULONG_MAX);
}

IoObject *IoNumber_unsignedIntMax(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number unsignedIntMax
	Returns the maximum unsigned int value.
	*/

	return IONUMBER(UINT_MAX);
}

IoObject *IoNumber_unsignedShortMax(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number unsignedShortMax
	Returns the minimum unsigned int value.
	*/

	return IONUMBER(USHRT_MAX);
}

IoObject *IoNumber_floatMax(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number floatMax
	Returns the maximum float value.
	*/

	return IONUMBER(FLT_MAX);
}

IoObject *IoNumber_floatMin(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number floatMin
	Returns the minimum float value.
	*/

	return IONUMBER(FLT_MIN);
}

IoObject *IoNumber_doubleMax(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number floatMax
	Returns the maximum double precision float value.
	*/

	return IONUMBER(DBL_MAX);
}

IoObject *IoNumber_doubleMin(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number doubleMin
	Returns the minimum double precision float value.
	*/

	return IONUMBER(DBL_MIN);
}

IoObject *IoNumber_isNan(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number isNan
	Returns true if the receiver is not a number. Otherwise returns false.
	*/

	return IOBOOL(self, isnan(CNUMBER(self)));
}

// looping ---------------------------------------------

IoObject *IoNumber_repeat(IoNumber *self, IoObject *locals, IoMessage *m)
{
	/*doc Number repeat(optionalIndex, expression)
	Evaluates message a number of times that corresponds to the receivers
	integer value. This is significantly  faster than a for() or while() loop.
	*/

	IoMessage_assertArgCount_receiver_(m, 1, self);

	{
		IoState *state = IOSTATE;
		IoSymbol *indexSlotName;
		IoMessage *doMessage;
		double i, max = CNUMBER(self);
		IoObject *result = IONIL(self);

		if(IoMessage_argCount(m) > 1)
		{
			indexSlotName = IoMessage_name(IoMessage_rawArgAt_(m, 0));
			doMessage = IoMessage_rawArgAt_(m, 1);
		}
		else
		{
			indexSlotName = 0;
			doMessage = IoMessage_rawArgAt_(m, 0);
		}

		IoState_pushRetainPool(state);

		for (i = 0; i < max; i ++)
		{
			/*
			if (result != locals && result != self)
			{
				IoState_immediatelyFreeIfUnreferenced_(state, result);
			}
			*/

			IoState_clearTopPool(state);

			if (indexSlotName)
			{
				IoObject_setSlot_to_(locals, indexSlotName, IONUMBER(i));
			}

			result = IoMessage_locals_performOn_(doMessage, locals, locals);

			if (IoState_handleStatus(IOSTATE))
			{
				break;
			}
		}

		IoState_popRetainPoolExceptFor_(IOSTATE, result);
		return result;
	}
}

