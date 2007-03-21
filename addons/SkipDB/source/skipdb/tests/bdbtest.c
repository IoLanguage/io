/*

cc bdbtest.c -Iinclude -Idist -L. -ldb -o test 

*/

#include "db.h"
#include "db_config.h"
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <db.h>
#include "db_config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX 1000
static double t1;

double mtime(void)
{
    double v;
    struct timeval tp;
    struct timezone tz;
    gettimeofday(&tp, &tz);
    v = tp.tv_sec;
    v += ((double)tp.tv_usec)/1000000.0;
    return v;
}

void test_startTimer(void)
{
    t1 = mtime();
}

double test_endTimer(int times, char *label)
{
    float d = mtime() - t1;
    
    if (label) 
    {
	int count = (((float)times) / d);
	printf("%i %s per second\n", count, label);
    }
    return d;
}

/*---------------------------------------------------*/

static char *path = "test.db";
static DB_ENV *dbenv; // DB environment pointer
static DB *dbp;       // DB pointer

static DBC *dbcp;     // DB cursor pointer
static DB_TXN *tid;   // transaction id
static int ret;

void test_beginTransaction(void)
{    
    if ((ret = txn_begin(dbenv, NULL, &tid, 0)) != 0) 
    {
	dbenv->err(dbenv, ret, "txn_begin");
	exit(1);
    }
}

void test_commitTransaction(void)
{    
    if ((ret = txn_commit(tid, DB_TXN_SYNC)) != 0) 
    {
	dbenv->err(dbenv, ret, "txn_commit");
	exit(1);
    }
    
    tid = NULL;
}

void test_setupEnvironment(void)
{
    struct stat sb;
    
    // If the directory exists, we're done.  We do not further check
    // the type of the file, DB will fail appropriately if it's the
    // wrong type.
    if (stat(path, &sb) == 0)
    {
	return;
    }
    
    // Create the directory, read/write/access owner only
    if (mkdir(path, S_IRWXU) != 0) 
    {
	fprintf(stderr, "txnapp: mkdir: %s: %s\n", path, strerror(errno));
	exit(1);
    }

    // Create the environment handle
    if ((ret = db_env_create(&dbenv, 0)) != 0) 
    {
	fprintf(stderr, "%s: db_env_create: %s\n", path, db_strerror(ret));
	exit(1);
    }
    
    // Set up error handling
    dbenv->set_errpfx(dbenv, path);
    
    //  Open a transactional environment:
    //  create if it doesn't exist
    //  free-threaded handle
    //  run recovery
    //  read/write owner only
    //printf("[directory cString] = %s\n", [directory cString]);
    if ((ret = dbenv->open(dbenv, path,
			   DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG |
			   DB_INIT_MPOOL | DB_INIT_TXN | DB_RECOVER, // DB_THREAD
			   S_IRUSR | S_IWUSR)) != 0) 
    {
	dbenv->err(dbenv, ret, "dbenv->open: %s", path);
	exit(1);
    }
}


void test_open(void)
{
    test_setupEnvironment();
    
    /* Create the database handle. */
    if ((ret = db_create(&dbp, dbenv, 0)) != 0) 
    {
	dbenv->err(dbenv, ret, "db_create");
	exit (1);
    }
    
    /*
     * Open a database in the environment:
     *  create if it doesn't exist
     *  free-threaded handle
     *  read/write owner only
     */
    if ((ret = dbp->open(dbp, "foo", NULL,
			 DB_BTREE, DB_CREATE, S_IRUSR | S_IWUSR)) != 0) 
    {
	dbenv->err(dbenv, ret, "dbp->open: %s", "foo");
	exit(1);
    }
    
    
}

void test_close(void)
{
    int ret;
    /*
     if ((ret = dbcp->c_close(dbcp)) != 0) {
	 dbp->err(dbp, ret, "DBcursor->close");
	 dbcp = NULL;
	 return;
     }
     */
    //[self sync];
    if ((ret = dbp->close(dbp, 0)) != 0) 
    {
	fprintf(stderr, "DB->close: %s\n", db_strerror(ret));
	dbcp = NULL;
    }
    
    if ((ret = dbenv->close(dbenv, 0)) != 0) 
    {
	fprintf(stderr, "dbenv->close: %s\n", db_strerror(ret));
	dbenv = NULL;
    }
}

void test_inserts(void)
{
    int i;
    DBT key;
    char skey[128];

    test_startTimer();
    
    for (i = MAX; i < MAX*2; i ++)
    {
	sprintf(skey, "%i", i);
	key.data = (void *)skey;
	key.size = strlen(skey);

	test_beginTransaction();
	ret = dbp->put(dbp, tid, &key, &key, DB_NOOVERWRITE);
	
	if (ret) 
	{
	    dbenv->err(dbenv, ret, "insert");
	    exit(1);
	}
	test_commitTransaction();
    }
    
    test_endTimer(MAX, "transactional inserts");
}

void test_reads(void)
{
    int i;
    DBT key, data;
    char skey[128];
    
    test_startTimer();
    
    for (i = MAX; i < MAX*2; i ++)
    {
	sprintf(skey, "%i", i);
	key.data = (void *)skey;
	key.size = strlen(skey);

	ret = dbp->get(dbp, NULL, &key, &data, 0);
	
	if (ret) 
	{
	    dbenv->err(dbenv, ret, "read");
	    exit(1);
	}
    }
    
    test_endTimer(MAX, "reads");
}

void test_remove(void)
{
    int i;
    DBT key;
    char skey[128];

    test_startTimer();
    
    for (i = MAX; i < MAX*2; i ++)
    {
	sprintf(skey, "%i", i);
	key.data = (void *)skey;
	key.size = strlen(skey);
	test_beginTransaction();

	ret = dbp->del(dbp, NULL, &key, 0);
	
	if (ret)
	{
	    dbp->err(dbp, ret, "DB->del");
	    exit(1);
	}

	test_commitTransaction();
    }
    test_endTimer(MAX, "transactional removes");
}



int main(int argc, const char * argv[]) 
{
    test_open();
    test_inserts();
    test_close();
    return 0;
}
