//  PHash.h
//  CuckooHashTable
//  Created by Steve Dekorte on 2009 04 28.

#ifndef PHASH_STRUCT_DEFINED
#define PHASH_STRUCT_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *k;
    void *v;
} PHashRecord;

typedef struct {
    unsigned char *records;
    size_t size;
    size_t keyCount;
    intptr_t mask;
} PHash;

#ifdef __cplusplus
}
#endif
#endif
