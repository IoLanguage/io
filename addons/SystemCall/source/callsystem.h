#ifndef CALLSYSTEM_INCLUDED
#define CALLSYSTEM_INCLUDED
/*
  callsystem    - system() on steorids (by ceteh?)

  features:
   - asynchonous running of a child process
   - setup of the environment
   - substitution of environment variables
   - connect all 3 standard streams to pipes, null devices, or files
   - pathname handling
   - #!shebang interpreter handling on any OS

  ALL IN A PORTABLE WAY!

  Pathnames:
   /   seperates a path,
   :   is used as device identifier when the path starts with / ("/C:" becomes "C:\")
   .   is the current directory
   ..  is the parent directory
   ~   is the users home directory

  Environment variables:
    only ${KEY} syntax is substituted in one pass, trying to substitute a non existing env var yields an error
 */

/*
  first abstraction of a few types and loading OS specific headers
*/

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

//#define unix 1
#if !defined(unix) && defined(__unix__) || defined(__NetBSD__)
#define unix
#endif

#if defined(__APPLE__)
	#define unix 1
	#include <sys/unistd.h>
#endif

#if defined(unix)
	#define _GNU_SOURCE
	#include <sys/time.h>
	#include <sys/resource.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/wait.h>
#elif defined(WIN32)
	#include <process.h>
#else
	#error "system not supported!"
#endif


#ifdef unix
/*
  opaque process identifier type and value for an illegal pid
 */
typedef pid_t callsystem_pid_t;
#define CALLSYSTEM_ILG_PID 0

/*
  opaque file descriptors and values for illegal, std and potable null fd's
  fd's are always used as pairs (two element array)
   [0] is the used for reading
   [1] is used for writing
   either side might be CALLSYSTEM_ILG_FD for non-pipes, callsystem cares for closing the aprobiate sides
*/
typedef int callsystem_fd_t;
/* this is never an fd */
#define CALLSYSTEM_ILG_FD -1

#elif defined(WIN32)
#include <windows.h>
typedef HANDLE callsystem_pid_t;
#define CALLSYSTEM_ILG_PID NULL
typedef HANDLE callsystem_fd_t;
#define CALLSYSTEM_ILG_FD INVALID_HANDLE_VALUE
#else
#error "system unsupported"
#endif



/*
  INVOKING AND CONTROLLING CHILD PROGRAMS
 */

int
callsystem(const char * cmd,
		   char * argv[],
		   char * env[],
		   callsystem_fd_t in[2],
		   callsystem_fd_t out[2],
		   callsystem_fd_t err[2],
		   const char * wd,
		   const int pri,
		   callsystem_pid_t * const child);

int
callsystem_running(callsystem_pid_t * pid);

int
callsystem_finished(callsystem_pid_t * pid);

int
callsystem_terminate(callsystem_pid_t *);

int
callsystem_abort(callsystem_pid_t *);


/*
  HANDLING FILE DESCRIPTORS AND PIPES
 */

int
callsystem_pipe(callsystem_fd_t pipe[2]);

int
callsystem_null(callsystem_fd_t null[2]);


enum callsystem_filemode{
	CALLSYSTEM_MODE_READ,
	CALLSYSTEM_MODE_WRITE,
	CALLSYSTEM_MODE_CREATE,
	CALLSYSTEM_MODE_APPEND,
	CALLSYSTEM_MODE_OVERWRITE,
	CALLSYSTEM_MODE_BINARY=8
};

int
callsystem_open(const char * filename,
				enum callsystem_filemode mode,
				callsystem_fd_t fd[2]);

/* want sockets too? prolly not in the domain of this thing */

int
callsystem_close(callsystem_fd_t fds[2]);

/*
  create a std C FILE stream to a underlying filedescriptor
*/
FILE *
callsystem_fdopen(callsystem_fd_t fds[2], enum callsystem_filemode mode);



/*
  ENVIRONMENT SETUP
 */

int
callsystem_setenv(char ** env[], const char * key, const char * value);

const char *
callsystem_getenv(char ** env[], const  char * key);

int
callsystem_unsetenv(char ** env[], const char * key);

int
callsystem_exportenv(char ** env[], const char * key);

int
callsystem_exportdefaults(char ** env[]);

char *
callsystem_env_subst(char ** envp[], const char * str);

void
callsystem_env_clear(char ** env[]);




/*
  SETUP OF THE ARGV
 */

int
callsystem_argv_pushback(char ** argv[], const char * const arg);

int
callsystem_argv_pushfront(char ** argv[], const char * const arg);

void
callsystem_argv_clear(char ** argv[]);


/*
  PORTABLE PATHNAME HANDLING
 */

/*
  planned:
  callsystem_canonalize_path_to_host()
  callsystem_canonalize_path_to_unix()

  convert unix like filenames to OS specific filenames (and back)

  checks that relative filenames do not leave their top directory

  for win32:
   "/Letter:/" becomes "Letter:\"
   "/foo/" becomes "\foo\"

   needs much to worked out (UNC?)

   callsystem_is_pathname()
	check if some string could be a legal pathname on the actual host
 */



#endif
