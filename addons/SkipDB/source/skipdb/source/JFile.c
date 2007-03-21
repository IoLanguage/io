/*#io
JFile ioDoc(
		  docCopyright("Steve Dekorte", 2004)
		  docLicense("BSD revised")
		  docObject("JFile")    
		  docDescription("A journaled file.")
		  */

#include "JFile.h"
#include "Common.h"	// for Win32 snprintf
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "PortableTruncate.h"

//#define DEBUG 1

void JFile_sync(JFile *self);
void JFile_readHeader(JFile *self);
void JFile_writeHeader(JFile *self);
void JFile_commitFinished(JFile *self);
int JFile_isCommitted(JFile *self);

JFile *JFile_new(void)
{
	JFile *self = (JFile *)calloc(1, sizeof(JFile));
	//JFile_setPath_(self, "default");
	return self;
}

void JFile_free(JFile *self)
{
	JFile_close(self);
	if (self->path) free(self->path);
	if (self->logPath) free(self->logPath);
	if (self->buf) free(self->buf);
	free(self);
}

void JFile_setPath_(JFile *self, const char *path)
{
	self->path    = strcpy((char *)realloc(self->path,    strlen(path)+1), path);
	self->logPath = strcpy((char *)realloc(self->logPath, strlen(path)+5), path);
	strcat(self->logPath,  ".log");
}

char *JFile_fileName(JFile *self)
{
#ifdef __WIN32__
	char *fileName = strrchr(self->path, '\\');
#else
	char *fileName = strrchr(self->path, '/');
#endif
	return  fileName ? fileName : self->path;
}

void JFile_setLogPath_(JFile *self, const char *path)
{
	char *fileName = JFile_fileName(self);
	self->logPath = strcpy((char *)realloc(self->logPath, strlen(path) + strlen(fileName)+10), path);
	if (self->logPath[strlen(self->logPath)-1] != '/') strcat(self->logPath, "/");
	strcat(self->logPath, fileName);
	strcat(self->logPath,  ".log");
}

void JFile_setPath_withExtension_(JFile *self, const char *path, const char *ext)
{
	size_t length = strlen(path) + 1 + strlen(ext) + 1;
	char *s = (char *)calloc(1, length);
	strcat(s, path);
	strcat(s, ".");
	strcat(s, ext);
	JFile_setPath_(self, s);
	free(s);
}

char *JFile_path(JFile *self)
{
	return self->path;
}

void JFile_open(JFile *self)
{
	JFile_openWithoutCommitCompletion(self);
	
	if (JFile_isCommitted(self)) 
	{
		JFile_sync(self); 
	}
}

void JFile_openWithoutCommitCompletion(JFile *self)
{
	self->file = fopen(self->path, "r+");
	
	if (!self->file) // if it doesn't exist, create file 
	{
		self->file = fopen(self->path, "w");
		fclose(self->file);
		self->file = fopen(self->path, "r+");
	}
	
	fseek(self->file, 0, SEEK_END);
	self->maxPos = ftell(self->file);
	
	self->log = fopen(self->logPath, "r+");
	//printf("%s\n", self->logPath);
	
	if (!self->log) // if it doesn't exist, create file 
	{
		self->log = fopen(self->logPath, "w");
		fclose(self->log);
		self->log = fopen(self->logPath, "r+");
	}
	
	self->header.state = JFILE_UNCOMMITTED;
	self->header.writeCount = 0;
	JFile_readHeader(self);
	// would normally auto commit here 
	self->logPos = sizeof(JFileLogHeader);
}

void JFile_close(JFile *self)
{
	if (self->file) 
	{
		fclose(self->file);
		self->file = NULL;
	}
	
	if (self->log) 
	{
		fclose(self->log);
		self->log = NULL;
	}
}

void JFile_delete(JFile *self)
{
	remove(self->path);
	remove(self->logPath);
}

size_t JFile_fwrite(JFile *self, void *buf, size_t size, size_t nobjs)
{
	size_t result;
	size_t total = size * nobjs;
	
#ifdef DEBUG
	if (total == 4 && strcmp(self->path, "default.udbData")) 
	{ 
		printf("%s at %i writing int %i\n", self->path, self->pos, *((int *)(buf))); 
	}
#endif
	
	fseek(self->log, self->logPos, SEEK_SET);
	fwrite(&(self->pos), sizeof(long), 1, self->log);
	fwrite(&total, sizeof(size_t), 1, self->log);
	
	
	result = fwrite(buf, size, nobjs, self->log);
	self->pos += total;
	if (self->pos > self->maxPos) self->maxPos = self->pos;
	self->logPos = ftell(self->log);
	self->needsSync = 1;
	
	self->header.writeCount ++;
	return result;
}


size_t JFile_fread(JFile *self, void *buf, size_t size, size_t nobjs)
{
	size_t result;
	
	// JFile_sync(self);  will leave file pos in proper place 
	fseek(self->file, self->pos, SEEK_SET);
	//printf("JFile read at %i %i\n", self->pos, ftell(self->file));
	result = fread(buf, size, nobjs, self->file);
	self->pos = ftell(self->file);
	
	
#ifdef DEBUG
	if (size == 4 && strcmp(self->path, "default.udbData")) 
	{ 
		printf("%s at %i read int %i\n", self->path, self->pos, *((int *)(buf))); 
	}
#endif
	
	return result;
}

int JFile_fseek(JFile *self, long offset, int origin)
{
	int result = fseek(self->file, offset, origin);
	self->pos = ftell(self->file);
	return result;
}

int JFile_fputc(JFile *self, int i)
{
	unsigned char c = i;
	JFile_fwrite(self, &c, 1, 1);
	return i;
}

void JFile_writeInt_(JFile *self, int v)
{
	JFile_fwrite(self, &v, sizeof(int), 1);
}

int JFile_readInt(JFile *self)
{
	int v;
	JFile_fread(self, &v, sizeof(int), 1);
	return v;
}

void JFile_setPosition_(JFile *self, long pos) 
{
	self->pos = pos; 
}

long JFile_position(JFile *self) 
{ 
	return self->pos; 
}

long JFile_setPositionToEnd(JFile *self)
{
	self->pos = self->maxPos;
	return self->pos;
}


#ifdef JFILE_SUPPORTS_MMAP

#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

size_t JFile_mmapfread(JFile *self, void *buf, size_t size, size_t nobjs)
{
	size_t result;
	int fd = fileno(self->file);
	size_t readSize = size * nobjs;
	size_t mapSize = self->pos + readSize;
	
	char *m = (char *)mmap(NULL, mapSize, PROT_READ, MAP_SHARED, fd, 0);
	
	if (m == MAP_FAILED || (int)m == -1)
	{
		perror("mmap");
		exit(-1);
	}
     
	memcpy(buf, m + self->pos, readSize); 
	munmap(m, mapSize);
	self->pos += readSize;
	result = readSize;
	
	return result;
}

void mmapread(void *buf, size_t size, size_t nobjs, char *m)
{
	size_t length = size * nobjs;
	memcpy(buf, m, length);
	m += length;
}

void JFile_sync(JFile *self)
{
	if (self->needsSync)
	{
		int i;
		long pos;
		size_t total;
		char *mroot, *m;
		size_t mapSize = self->logPos;
		int fd = fileno(self->log);
		
		//printf("mapSize = %i\n", (int)mapSize);
		
		JFile_flushLog(self);
		self->logPos = sizeof(JFileLogHeader);
		fseek(self->log, self->logPos, SEEK_SET);
		
		mroot = (char *)mmap(NULL, mapSize, PROT_READ, MAP_SHARED, fd, 0);
		m = mroot;
		assert(m != MAP_FAILED || (int)m != -1);
		
		for (i = 0; i < self->header.writeCount; i ++)
		{
			//fread(&pos,   sizeof(long), 1, self->log);
			memcpy(&pos, m, sizeof(long)); m += sizeof(long);
			
			//fread(&total, sizeof(size_t), 1, self->log);
			memcpy(&total, m, sizeof(size_t)); m += sizeof(size_t);
			
			self->buf = (unsigned char *)realloc(self->buf, total);
			//fread(self->buf, total, 1, self->log);
			memcpy(self->buf, m, total); m += total;
			
			fseek(self->file, pos, SEEK_SET);
			fwrite(self->buf, total, 1, self->file);
		}
		
		munmap(mroot, mapSize);
		
		self->needsSync = 0;
		JFile_flushFile(self);;
		fseek(self->file, self->pos, SEEK_SET);
		fseek(self->log, self->logPos, SEEK_SET);
		
		JFile_commitFinished(self);
#ifdef DEBUG
		printf("--------------------------------------------------\n");
#endif
	}
}

#else

void JFile_sync(JFile *self)
{
	if (self->needsSync)
	{
		int i;
		long pos;
		size_t total;
		
		//JFile_flushLog(self); // needed?
		self->logPos = sizeof(JFileLogHeader);
		fseek(self->log, self->logPos, SEEK_SET);
		
#ifdef DEBUG
		if (strcmp(self->path, "default.udbData") != 0) 
		{ 
			printf("------- %s - commiting %i writes -----\n", self->path, self->header.writeCount); 
		}
#endif
		
		for (i = 0; i < self->header.writeCount; i ++)
		{
			fread(&pos,   sizeof(long), 1, self->log);
			fread(&total, sizeof(size_t), 1, self->log);
			self->buf = (unsigned char *)realloc(self->buf, total);
			fread(self->buf, total, 1, self->log);
			
#ifdef DEBUG
			if (total == 4 && strcmp(self->path, "default.udbData")) 
			{ 
				printf("%s at %i committing int %i\n", self->path, pos, *((int *)(self->buf))); 
			}
#endif
			
			fseek(self->file, pos, SEEK_SET);
			fwrite(self->buf, total, 1, self->file);
		}
		
		self->needsSync = 0;
		JFile_flushFile(self);;
		fseek(self->file, self->pos,    SEEK_SET);
		fseek(self->log,  self->logPos, SEEK_SET);
		
		JFile_commitFinished(self); // begin will do this!
#ifdef DEBUG
		printf("--------------------------------------------------\n");
#endif
	}
}

#endif

// header ------------------------ 

void JFile_readHeader(JFile *self)
{
	rewind(self->log);
	fread((unsigned char *)(&(self->header)), sizeof(JFileLogHeader), 1, self->log);
}

void JFile_writeHeader(JFile *self)
{
	rewind(self->log);
	fwrite((unsigned char *)(&(self->header)), sizeof(JFileLogHeader), 1, self->log);
	JFile_flushLog(self);
}

void JFile_begin(JFile *self)
{
	self->header.state = JFILE_UNCOMMITTED;
	self->header.writeCount = 0;
	JFile_writeHeader(self);
	//JFile_sync(self);
}

void JFile_commit(JFile *self)
{
	self->header.state = JFILE_COMMITTED;
	JFile_writeHeader(self);
	JFile_sync(self);
}

void JFile_clipLog(JFile *self)
{
	fclose(self->log);
	truncate(self->logPath, sizeof(JFileLogHeader));
	self->log = fopen(self->logPath, "r+b");
}

void JFile_commitFinished(JFile *self)
{
	/*JFile_clipLog(self);*/
	self->header.state = JFILE_UNCOMMITTED;
	self->header.writeCount = 0;
	JFile_writeHeader(self);
}

int JFile_isCommitted(JFile *self)
{
	JFile_readHeader(self);
	return (self->header.state == JFILE_COMMITTED);
}

void JFile_truncate_(JFile *self, off_t size)
{
	long pos = self->pos;
	JFile_commit(self);
	JFile_close(self);
	//printf("truncate(%s, %.0f)\n", self->path, (float)size);
	truncate(self->path, size);
	JFile_open(self);
	fseek(self->file, pos, SEEK_SET);
}

int JFile_Test(void)
{
	JFile *t = JFile_new();
	
	JFile_setPath_(t, "testing");
	JFile_open(t);
	JFile_begin(t);
	
	{
		int i;
		
		for (i = 0; i < 3; i ++)
		{
			char s[128];
			
			snprintf(s, 128, "foobar-%i", i);
			printf("write: %s\n", s);
			JFile_fwrite(t, s, strlen(s), 1);
		}
	}
	
	printf("commit\n");
	JFile_commit(t);
	printf("done\n");
	JFile_close(t);
	JFile_free(t);
	return 0;
}

/*
 See:
 http://lists.apple.com/archives/darwin-dev/2005/Feb/msg00072.html
 */

#include <fcntl.h>

void JFile_flushLog(JFile *self)
{
	//int fd = fileno(self->log);
	fflush(self->log);
	//fcntl(fd, F_FULLFSYNC, NULL);
}

void JFile_flushFile(JFile *self)
{
	//int fd = fileno(self->file);
	fflush(self->file);
	//fcntl(fd, F_FULLFSYNC, NULL);
}
