/*#io
docCopyright("Steve Dekorte", 2004)
docLicense("BSD revised")
docObject("SkipDBM")    
docDescription("A SkipDB Manager.")
*/

#ifndef SkipDBM_DEFINED
#define SkipDBM_DEFINED 1

#include "SkipDB.h"
#include "Hash.h"
#include <stdio.h>
#include <sys/types.h> 

#ifdef __cplusplus
extern "C" {
#endif

typedef void (SkipDBMThreadLockFunc)(void *);
typedef void (SkipDBMThreadUnlockFunc)(void *);

typedef struct
{
	UDB *udb;
	Hash *pidToDB;
	List *dbs;
	SkipDB *rootDB;
	void *callbackContext;
	SkipDBMThreadLockFunc *threadLockCallback;
	SkipDBMThreadUnlockFunc *threadUnlockCallback;
} SkipDBM;

SKIPDB_API SkipDBM *SkipDBM_new(void);
SKIPDB_API void SkipDBM_free(SkipDBM *self);

SKIPDB_API UDB *SkipDBM_udb(SkipDBM *self);

SKIPDB_API void SkipDBM_setPath_(SkipDBM *self, const char *path);
SKIPDB_API char *SkipDBM_path(SkipDBM *self);

// open/close

SKIPDB_API void SkipDBM_open(SkipDBM *self);
SKIPDB_API int SkipDBM_isOpen(SkipDBM *self);
SKIPDB_API void SkipDBM_close(SkipDBM *self);
SKIPDB_API void SkipDBM_delete(SkipDBM *self);

// databases 

SKIPDB_API SkipDB *SkipDBM_newSkipDB(SkipDBM *self);
SKIPDB_API SkipDB *SkipDBM_rootSkipDB(SkipDBM *self);
SKIPDB_API SkipDB *SkipDBM_skipDBAtPid_(SkipDBM *self, PID_TYPE pid);
SKIPDB_API void SkipDBM_willFreeDB_(SkipDBM *self, SkipDB *sdb); // private 

// transactions

SKIPDB_API void SkipDBM_beginTransaction(SkipDBM *self);
SKIPDB_API void SkipDBM_commitTransaction(SkipDBM *self);

// cache 

SKIPDB_API void SkipDBM_clearCaches(SkipDBM *self);

// compact 

SKIPDB_API int SkipDBM_compact(SkipDBM *self);

// thread locking

SKIPDB_API void SkipDBM_setCallbackContext_(SkipDBM *self, void *calbackContext);

SKIPDB_API void SkipDBM_setThreadLockCallback_(SkipDBM *self, SkipDBMThreadLockFunc *calback);
SKIPDB_API void SkipDBM_setThreadUnlockCallback_(SkipDBM *self, SkipDBMThreadUnlockFunc *calback);

SKIPDB_API void SkipDBM_lockThread(SkipDBM *self);
SKIPDB_API void SkipDBM_unlockThread(SkipDBM *self);

#ifdef __cplusplus
}
#endif
#endif
