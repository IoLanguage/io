#ifndef _IO_OBJBC_BRIDGE_HASH
#define _IO_OBJBC_BRIDGE_HASH

#include "CHash_ObjcBridgeFunctions.h"
#include "CHash.h"
#include "PortableStdint.h"
#include "Hash_superfast.h"
#include "Hash_murmur.h"


int Pointer_equals_(void *v1, void *v2)
{
	return (uintptr_t)v1 == (uintptr_t)v2;
}

uintptr_t Pointer_superfastHash(const void *v)
{
	return SuperFastHash((char *)&v, sizeof(void *));
}

uintptr_t Pointer_murmurHash(const void *v)
{
	return (uintptr_t)MurmurHash2((const void *)&v, sizeof(void *), 0);
}

void CHash_setObjcBridgeHashFunctions(CHash* hash)
{
    CHash_setEqualFunc_(hash, (CHashEqualFunc *)Pointer_equals_);
    CHash_setHash1Func_(hash, (CHashHashFunc *)Pointer_superfastHash);
    CHash_setHash2Func_(hash, (CHashHashFunc *)Pointer_murmurHash);
}

#endif
