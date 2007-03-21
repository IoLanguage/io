/*#io
SkipDBM ioDoc(
		    docCopyright("Steve Dekorte", 2004)
		    docLicense("BSD revised")
		    docObject("SkipDBM")    
		    docDescription("A SkipDB Manager.")
		    */

#include "SkipDBM.h"

SkipDBM *SkipDBM_new(void)
{
	SkipDBM *self = calloc(1, sizeof(SkipDBM));
	self->udb = UDB_new();
	self->dbs = List_new();
	self->pidToDB = Hash_new();
	SkipDBM_setPath_(self, "default");
	self->rootDB = SkipDB_newWithDBM_(self);
	//self->rootDB = SkipDB_newWithDBM_atPid_(self, 1);
	List_append_(self->dbs, self->rootDB);
	return self;
}

void SkipDBM_freeDBs(SkipDBM *self)
{
	//Hash_do_(self->pidToDB, (HashDoCallback *)SkipDB_dealloc);
	LIST_FOREACH(self->dbs, i, sdb, SkipDB_sdbm_(sdb, NULL));
	List_do_(self->dbs, (ListDoCallback *)SkipDB_release); 
	List_removeAll(self->dbs);
}

void SkipDBM_free(SkipDBM *self)
{
	SkipDBM_freeDBs(self);
	List_free(self->dbs);
	Hash_free(self->pidToDB);
	UDB_free(self->udb);
	free(self);
}

void SkipDBM_setPath_(SkipDBM *self, const char *path)
{
	UDB_setPath_(self->udb, path);
}

char *SkipDBM_path(SkipDBM *self) 
{ 
	return UDB_path(self->udb); 
}

UDB *SkipDBM_udb(SkipDBM *self)
{
	return self->udb;
}

int SkipDBM_isOpen(SkipDBM *self)
{
	return UDB_isOpen(self->udb);
}

void SkipDBM_open(SkipDBM *self)
{
	UDB_open(self->udb);
	
	if (!self->rootDB)
	{
		//self->rootDB = SkipDB_newWithDBM_atPid_(self, 1);
		self->rootDB = SkipDB_newWithDBM_(self);
		List_append_(self->dbs, self->rootDB);
	}
}

void SkipDBM_close(SkipDBM *self)
{
	SkipDBM_freeDBs(self);
	UDB_close(self->udb);
	self->rootDB = NULL;
}

void SkipDBM_delete(SkipDBM *self)
{
	UDB_close(self->udb);
	UDB_delete(self->udb);
}

// databases --------------------------------------------------- 

SkipDB *SkipDBM_newSkipDB(SkipDBM *self)
{
	SkipDB *sdb = SkipDB_newWithDBM_((void *)self);
	
	return sdb;
}

SkipDB *SkipDBM_rootSkipDB(SkipDBM *self)
{
	// SkipDBM_skipDBAtPid_(self, 1);
	return self->rootDB;
}

SkipDB *SkipDBM_skipDBAtPid_(SkipDBM *self, PID_TYPE pid)
{
	SkipDB *sdb;
	
	sdb = Hash_at_(self->pidToDB, (void *)pid);
	
	if (!sdb)
	{
		sdb = SkipDB_newWithDBM_atPid_(self->udb, pid); 
	}
	
	return sdb;
}

void SkipDBM_willFreeDB_(SkipDBM *self, SkipDB *sdb)
{
	PID_TYPE pid = SkipDB_headerPid(sdb);
	
	if (pid)
	{
		Hash_removeKey_(self->pidToDB, (void *)pid);
	}
	
	List_remove_(self->dbs, sdb);
}

// transactions ------------------------------------------ 

void SkipDBM_beginTransaction(SkipDBM *self)
{
	if (SkipDBM_isOpen(self)) 
	{
		UDB_beginTransaction(self->udb);
	}
}

void SkipDBM_commitTransaction(SkipDBM *self)
{
	if (SkipDBM_isOpen(self)) 
	{
		List_do_(self->dbs, (ListDoCallback *)SkipDB_sync);
		UDB_commitTransaction(self->udb);
	}
}

// cache ---------------------------------------------

void SkipDBM_clearCache(SkipDBM *self)
{
	Hash_do_(self->pidToDB, (HashDoCallback *)SkipDB_clearCache);
}

// compact ------------------------------------------------ 

int SkipDBM_compact(SkipDBM *self)
{
	return UDB_compact(self->udb);
}

// thread locking

void SkipDBM_setCallbackContext_(SkipDBM *self, void *callbackContext)
{
	self->callbackContext = callbackContext;
}

void SkipDBM_setThreadLockCallback_(SkipDBM *self, SkipDBMThreadLockFunc *callback)
{
	self->threadLockCallback = callback;
}

void SkipDBM_setThreadUnlockCallback_(SkipDBM *self, SkipDBMThreadUnlockFunc *callback)
{
	self->threadUnlockCallback = callback;
}

void SkipDBM_lockThread(SkipDBM *self)
{
	if (self->threadLockCallback)
	{
		self->threadLockCallback(self->callbackContext);
	}
}

void SkipDBM_unlockThread(SkipDBM *self)
{
	if (self->threadUnlockCallback)
	{
		self->threadUnlockCallback(self->callbackContext);
	}
}
