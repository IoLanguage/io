/*************************************************************************************************
* Writing test of Quick Database Manager
*************************************************************************************************/


#include <depot.h>
#include <cabin.h>
#include <villa.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#undef TRUE
#define TRUE           1                 /* boolean true */
#undef FALSE
#define FALSE          0                 /* boolean false */

#define RECBUFSIZ      32                /* buffer for records */


/* global variables */
const char *progname;                    /* program name */
int showprgr;                            /* whether to show progression */


/* function prototypes */
int main(int argc, char **argv);
void usage(void);
int runwrite(int argc, char **argv);
int runread(int argc, char **argv);
int runbtwrite(int argc, char **argv);
int runbtread(int argc, char **argv);
int myrand(void);
int dowrite(char *name, int rnum);
int doread(char *name, int rnum);
int dobtwrite(char *name, int rnum, int rnd);
int dobtread(char *name, int rnum, int rnd);


/* main routine */
int main(int argc, char **argv){
    int rv;
    progname = argv[0];
    showprgr = TRUE;
    if(getenv("HIDEPRGR")) showprgr = FALSE;
    if(argc < 2) usage();
    rv = 0;
    if(!strcmp(argv[1], "write")){
	rv = runwrite(argc, argv);
    } else if(!strcmp(argv[1], "read")){
	rv = runread(argc, argv);
    } else if(!strcmp(argv[1], "btwrite")){
	rv = runbtwrite(argc, argv);
    } else if(!strcmp(argv[1], "btread")){
	rv = runbtread(argc, argv);
    } else {
	usage();
    }
    return rv;
}


/* print the usage and exit */
void usage(void){
    fprintf(stderr, "%s: test cases for Quick Database Manager\n", progname);
    fprintf(stderr, "\n");
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s write name rnum\n", progname);
    fprintf(stderr, "  %s read name rnum\n", progname);
    fprintf(stderr, "  %s btwrite [-rnd] name rnum\n", progname);
    fprintf(stderr, "  %s btread [-rnd] name rnum\n", progname);
    exit(1);
}


/* parse arguments of write command */
int runwrite(int argc, char **argv){
    char *name, *rstr;
    int i, rnum, rv;
    name = NULL;
    rstr = NULL;
    rnum = 0;
    for(i = 2; i < argc; i++){
	if(!name && argv[i][0] == '-'){
	    usage();
	} else if(!name){
	    name = argv[i];
	} else if(!rstr){
	    rstr = argv[i];
	} else {
	    usage();
	}
    }
    if(!name || !rstr) usage();
    rnum = atoi(rstr);
    if(rnum < 1) usage();
    rv = dowrite(name, rnum);
    return rv;
}


/* parse arguments of read command */
int runread(int argc, char **argv){
    char *name, *rstr;
    int i, rnum, rv;
    name = NULL;
    rstr = NULL;
    rnum = 0;
    for(i = 2; i < argc; i++){
	if(!name && argv[i][0] == '-'){
	    usage();
	} else if(!name){
	    name = argv[i];
	} else if(!rstr){
	    rstr = argv[i];
	} else {
	    usage();
	}
    }
    if(!name || !rstr) usage();
    rnum = atoi(rstr);
    if(rnum < 1) usage();
    rv = doread(name, rnum);
    return rv;
}


/* parse arguments of btwrite command */
int runbtwrite(int argc, char **argv){
    char *name, *rstr;
    int i, rnum, rnd, rv;
    name = NULL;
    rstr = NULL;
    rnum = 0;
    rnd = FALSE;
    for(i = 2; i < argc; i++){
	if(!name && argv[i][0] == '-'){
	    if(!name && !strcmp(argv[i], "-rnd")){
		rnd = TRUE;
	    } else {
		usage();
	    }
	} else if(!name){
	    name = argv[i];
	} else if(!rstr){
	    rstr = argv[i];
	} else {
	    usage();
	}
    }
    if(!name || !rstr) usage();
    rnum = atoi(rstr);
    if(rnum < 1) usage();
    rv = dobtwrite(name, rnum, rnd);
    return rv;
}


/* parse arguments of btread command */
int runbtread(int argc, char **argv){
    char *name, *rstr;
    int i, rnum, rnd, rv;
    name = NULL;
    rstr = NULL;
    rnum = 0;
    rnd = FALSE;
    for(i = 2; i < argc; i++){
	if(!name && argv[i][0] == '-'){
	    if(!name && !strcmp(argv[i], "-rnd")){
		rnd = TRUE;
	    } else {
		usage();
	    }
	} else if(!name){
	    name = argv[i];
	} else if(!rstr){
	    rstr = argv[i];
	} else {
	    usage();
	}
    }
    if(!name || !rstr) usage();
    rnum = atoi(rstr);
    if(rnum < 1) usage();
    rv = dobtread(name, rnum, rnd);
    return rv;
}


/* pseudo random number generator */
int myrand(void){
    static int cnt = 0;
    return (rand() * rand() + (rand() >> (sizeof(int) * 4)) + (cnt++)) & 0x7FFFFFFF;
}


/* perform write command */
int dowrite(char *name, int rnum){
    DEPOT *depot;
    int i, err, len;
    char buf[RECBUFSIZ];
    if(showprgr) printf("<Writing Test of Hash>\n  name=%s  rnum=%d\n\n", name, rnum);
    /* open a database */
    if(!(depot = dpopen(name, DP_OWRITER | DP_OCREAT | DP_OTRUNC, rnum * 3))){
	fprintf(stderr, "dpopen failed\n");
	return 1;
    }
    err = FALSE;
    /* loop for each record */
    for(i = 1; i <= rnum; i++){
	/* store a record */
	len = sprintf(buf, "%08d", i);
	if(!dpput(depot, buf, len, buf, len, DP_DOVER)){
	    fprintf(stderr, "dpput failed\n");
	    err = TRUE;
	    break;
	}
	/* print progression */
	if(showprgr && rnum > 250 && i % (rnum / 250) == 0){
	    putchar('.');
	    fflush(stdout);
	    if(i == rnum || i % (rnum / 10) == 0){
		printf(" (%08d)\n", i);
		fflush(stdout);
	    }
	}
    }
    /* close the database */
    if(!dpclose(depot)){
	fprintf(stderr, "dpclose failed\n");
	return 1;
    }
    if(showprgr && !err) printf("ok\n\n");
    return err ? 1 : 0;
}


/* perform read command */
int doread(char *name, int rnum){
    DEPOT *depot;
    int i, err, len;
    char buf[RECBUFSIZ], vbuf[RECBUFSIZ];
    if(showprgr) printf("<Reading Test of Hash>\n  name=%s  rnum=%d\n\n", name, rnum);
    /* open a database */
    if(!(depot = dpopen(name, DP_OREADER, -1))){
	fprintf(stderr, "dpopen failed\n");
	return 1;
    }
    err = FALSE;
    /* loop for each record */
    for(i = 1; i <= rnum; i++){
	/* store a record */
	len = sprintf(buf, "%08d", i);
	if(dpgetwb(depot, buf, len, 0, RECBUFSIZ, vbuf) == -1){
	    fprintf(stderr, "dpget failed\n");
	    err = TRUE;
	    break;
	}
	/* print progression */
	if(showprgr && rnum > 250 && i % (rnum / 250) == 0){
	    putchar('.');
	    fflush(stdout);
	    if(i == rnum || i % (rnum / 10) == 0){
		printf(" (%08d)\n", i);
		fflush(stdout);
	    }
	}
    }
    /* close the database */
    if(!dpclose(depot)){
	fprintf(stderr, "dpclose failed\n");
	return 1;
    }
    if(showprgr && !err) printf("ok\n\n");
    return err ? 1 : 0;
}

#include <sys/time.h>     

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

/* perform btwrite command */
int dobtwrite(char *name, int rnum, int rnd)
{
    double t1, t2, dt;
    
    VILLA *villa;
    int i, err, len;
    char buf[RECBUFSIZ];
    
    if(showprgr) printf("<Writing Test of B+ Tree>\n  name=%s  rnum=%d\n\n", name, rnum);
    
    /* open a database */
    if(!(villa = vlopen(name, VL_OWRITER | VL_OCREAT | VL_OTRUNC, VL_CMPLEX))){
	fprintf(stderr, "vlopen failed\n");
	return 1;
    }
    if(rnd){
	vlsettuning(villa, 37, 200, rnum / 45, 512);
    } else {
	vlsettuning(villa, 101, 256, 16, 16);
    }
    err = FALSE;
    
    t1 = mtime();
    /* loop for each record */
    for(i = 1; i <= rnum; i++)
    {
	/* store a record */
	len = sprintf(buf, "%08d", rnd ? myrand() % rnum + 1 : i);

	vltranbegin(villa);

	if(!vlput(villa, buf, len, buf, len, VL_DOVER)){
	    fprintf(stderr, "vlput failed\n");
	    err = TRUE;
	    break;
	}

	vltrancommit(villa);

	/* print progression */
	if(showprgr && rnum > 250 && i % (rnum / 250) == 0){
	    putchar('.');
	    fflush(stdout);
	    if(i == rnum || i % (rnum / 10) == 0){
		printf(" (%08d)\n", i);
		fflush(stdout);
	    }
	}
    }
    t2 = mtime();
    dt = t2 - t1;
    printf("%i transactional write in %f seconds\n", rnum, (float)dt);
    printf("%i transactional writes per second\n", (int)(((float)rnum)/((float)dt)));
    /* close the database */
    if(!vlclose(villa)){
	fprintf(stderr, "vlclose failed\n");
	return 1;
    }
    if(showprgr && !err) printf("ok\n\n");
    return err ? 1 : 0;
}


/* perform btread command */
int dobtread(char *name, int rnum, int rnd){
    double t1, t2, dt;
    VILLA *villa;
    int i, err, len;
    char buf[RECBUFSIZ], *val;
    if(showprgr) printf("<Reading Test of B+ Tree>\n  name=%s  rnum=%d\n\n", name, rnum);
    /* open a database */
    if(!(villa = vlopen(name, VL_OREADER, VL_CMPLEX))){
	fprintf(stderr, "vlopen failed\n");
	return 1;
    }
    if(rnd){
	vlsettuning(villa, 37, 200, rnum / 45, 512);
    } else {
	vlsettuning(villa, 101, 256, 16, 16);
    }
    err = FALSE;
    /* loop for each record */
    t1 = mtime();

    for(i = 1; i <= rnum; i++){
	/* store a record */
	len = sprintf(buf, "%08d", rnd ? myrand() % rnum + 1 : i);
	if(!(val = vlget(villa, buf, len, NULL))){
	    fprintf(stderr, "vlget failed\n");
	    err = TRUE;
	    break;
	}
	free(val);
	/* print progression */
	if(showprgr && rnum > 250 && i % (rnum / 250) == 0){
	    putchar('.');
	    fflush(stdout);
	    if(i == rnum || i % (rnum / 10) == 0){
		printf(" (%08d)\n", i);
		fflush(stdout);
	    }
	}
    }
    
    t2 = mtime();
    dt = t2 - t1;
    printf("%i reads in %f seconds\n", rnum, (float)dt);
    printf("%i reads per second\n", (int)(((float)rnum)/((float)dt)));
    
    /* close the database */
    if(!vlclose(villa)){
	fprintf(stderr, "vlclose failed\n");
	return 1;
    }
    if(showprgr && !err) printf("ok\n\n");
    return err ? 1 : 0;
}



/* END OF FILE */
