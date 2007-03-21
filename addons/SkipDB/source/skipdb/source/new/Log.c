/*   
    copyright: Steve Dekorte, 2004. All rights reserved.
    license: See _BSDLicense.txt.

    object: Log
    module: Log
    description: 

    File format is:
    
	status - sizeof(uint8_t)
    
    followed by N number of entries of the type:
    
	size - sizeof(size_t)
	data - size bytes long
    
*/
 
#include "Log.h"
#include "Common.h"	// for Win32 snprintf
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "PortableTruncate.h"

Log *Log_new(void)
{
    Log *self = (Log *)calloc(1, sizeof(Log));
    /*Log_setPath_(self, "default");*/
    return self;
}

void Log_free(Log *self)
{
    Log_close(self);
    if (self->path) free(self->path);
    if (self->readDataBuffer) free(self->readDataBuffer);
    free(self);
}

void Log_setPath_(Log *self, char *path)
{
    self->path = strcpy((char *)realloc(self->path,    strlen(path)+1), path);
}

char *Log_path(Log *self)
{
    return path;
}

void Log_error_(Log *self, char *s)
{
    printf("Log error: %s\n", s);
}

void Log_open(Log *self)
{
    self->fp = fopen(self->path, "r+b");
    
    /* if it doesn't exist, we create it and then reopen it */
    if (!self->fp) 
    {
	self->fp = fopen(self->path, "wb");
	fclose(self->file);
	self->fp = fopen(self->path, "r+b");
	Log_setStatus_(self, 0x0);
    }
}

void Log_close(Log *self)
{
    if (self->fp) 
    {
	fclose(self->fp);
	self->fp = 0x0;
    }
}

void Log_delete(Log *self)
{
    remove(self->path);
}

/* --- status --- */

uint8_t Log_status(Log *self)
{
    uint8_t status;
    fseek(self->fp, 0, SEEK_SET); /* move to beginning */
    fread(&status, 1, 1, self->fp)
    return status;
}

void Log_setStatus_(Log *self, uint8_t s)
{
    uint8_t status;
    fseek(self->fp, 0, SEEK_SET); /* move to beginning */
    fread(&status, 1, 1, self->fp)
    return status;
}

/* --- writing --- */

void Log_removeAllRecords(Log *self)
{
    truncate(self->fp, 1); /* remove all but the status byte */
}

size_t Log_addDatum_(Log *self, Datum d)
{
    fseek(self->fp, 0, SEEK_END);
    fwrite(&(d.size), sizeof(PID_TYPE), 1, self->fp);
    fwrite(d.data, 1, d.size, self->fp);
}

/* --- reading --- */

void Log_setReadPosition_(Log *self, long pos)
{
    self->readPos = pos;
}

void Log_rewind(Log *self)
{
    /* move read position to beginning of file after status */
    Log_setReadPosition_(self, 1);
}

Datum Log_nextDatum(Log *self)
{
    Datum d = Log_datumAt_(self, self->readPos);
    self->readPos = ftell(self->fp);
    return d;
}

Datum Log_datumAt_(Log *self, long filePosition)
{
    Datum d;
    int bytesRead = 0;
    
    /* read the size */
    
    fseek(self->fp, filePosition, SEEK_SET);
    bytesRead = fread(&(d.size), sizeof(PID_TYPE), 1, self->fp);
    
    if (bytesRead != sizeof(PID_TYPE))
    {
	d.size = 0;
	d.data = 0x0;
	
	if (bytesRead != 0)
	{
	    Log_error_(self, "unexpected end of file while reading entry size");
	}
	
	return d;
    }
    
    /* read the data */
    
    self->readBuffer = realloc(self->readBuffer, d.size);
    bytesRead = fread(self->readBuffer, d.size, 1, self->fp);

    if (bytesRead != d.size)
    {
	d.size = 0;
	d.data = 0x0;
	Log_error_(self, "unexpected end of file while reading entry data");
	return d;
    }
    
    d.data = readBuffer;
    return d;
}
