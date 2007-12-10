/*#io
docCopyright("Steve Dekorte", 2004)
docLicense("BSD revised")
docObject("UDB")    
docDescription("An unordered value database. (sort of like malloc for disk space)
    It compacts the data like a single space copying garbage collector.")
*/

#ifndef UDB_DEFINED
#define UDB_DEFINED 1

#include "List.h"
#include "ByteArray.h"
#include "UDBRecord.h"
#include "UDBIndex.h"
#include "UDBRecords.h"
#include "SkipDB.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    char *path;
    UDBIndex *index;
    UDBRecords *records;
    int withinTransaction;
    int isOpen;
} UDB;

SKIPDB_API UDB *UDB_new(void);
SKIPDB_API void UDB_free(UDB *self);

SKIPDB_API void UDB_setPath_(UDB *self, const char *s);
SKIPDB_API void UDB_setLogPath_(UDB *self, const char *s);
SKIPDB_API char *UDB_path(UDB *self);

SKIPDB_API void UDB_delete(UDB *self);
SKIPDB_API void UDB_open(UDB *self);
SKIPDB_API int UDB_isOpen(UDB *self);
SKIPDB_API void UDB_close(UDB *self);

// transactions --------------------------------------------------- 

SKIPDB_API void UDB_beginTransaction(UDB *self);
SKIPDB_API void UDB_commitTransaction(UDB *self);

// ops -------------------------------------------------- 

SKIPDB_API PID_TYPE UDB_nextPid(UDB *self);
SKIPDB_API PID_TYPE UDB_allocPid(UDB *self);

SKIPDB_API PID_TYPE UDB_put_(UDB *self, Datum d);
SKIPDB_API void UDB_at_put_(UDB *self, PID_TYPE pid, Datum d);
SKIPDB_API Datum UDB_at_(UDB *self, PID_TYPE pid);
SKIPDB_API void UDB_removeAt_(UDB *self, PID_TYPE id);

SKIPDB_API int UDB_compact(UDB *self);
SKIPDB_API int UDB_compactStep(UDB *self);
SKIPDB_API int UDB_compactStepFor_(UDB *self, double maxSeconds);

SKIPDB_API void UDB_show(UDB *self);
SKIPDB_API void UDB_showIndex(UDB *self);

#ifdef __cplusplus
}
#endif
#endif
