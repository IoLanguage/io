/*   
    copyright: Steve Dekorte, 2005. All rights reserved.
    license: See _BSDLicense.txt.
*/

#ifndef Log_DEFINED
#define Log_DEFINED 1

#include <stdio.h>
#include <sys/types.h> 
#include "Datum.h" 

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{    
    char *path;
    FILE *fp;
 
    char status;

    long readPos;
    uint8_t *readDataBuffer;
} Log;

Log *Log_new(void);
void Log_free(Log *self);

void Log_setPath_(Log *self, char *path);
char *Log_path(Log *self);

void Log_open(Log *self);
void Log_close(Log *self);
void Log_delete(Log *self);

/* --- status --- */

uint8_t Log_status(Log *self);
void Log_setStatus_(Log *self, uint8_t s);

/* --- writing --- */

void Log_removeAllRecords(Log *self);
size_t Log_addDatum_(Log *self, Datum d);

/* --- reading --- */

void Log_setReadPosition_(Log *self, long pos);
void Log_rewind(Log *self);
Datum Log_nextDatum(Log *self); /* if returned datum size is zero, end of file was reached */
Datum Log_datumAt_(Log *self, long filePosition);

#ifdef __cplusplus
}
#endif
#endif
