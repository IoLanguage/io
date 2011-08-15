/*
 callsystem    - system() on steorids

 features:
 - asynchonous running of a child process
 - setup of the environment
 - substitution of environment variables
 - connect all 3 standard streams to pipes, null devices, or files
 - pathname handling
 - #!shebang interpreter handling on any OS

 ALL IN A PORTABLE WAY!

 Note: the Win32 implementation hasnt yet the exact same semantic as the unix stuff,
 feel free to send me patches <chth@gmx.net>. The unix semantics are the mandatory semantics,
 don't change them!

 Pathnames:
 /   seperates a path,
 :   is used as device identifier when the path starts with /("/C:/" becomes "C:\")
 .   is the current directory
 ..  is the parent directory

 Environment variables:
 only ${KEY} syntax is substituted in one pass, trying to substitute a non existing env var yields an error

 Note on error handling:
 Function returns follow the unix conventions which means:
 for int as return value:
 return 0 in case of success
 return -1 in case of an error
 for pointers as return value:
 return a valid pointer on success
 return NULL in case of an error

 To retrieve the actual error code one still needs to use the system specific method which is:
 on unix:  errno
 on Win32: GetLastError()

 NB: this was in the original comment and is only half correct
 "Note: errno is not set by Win32 API calls and is global(not thread local)"
 however the ms crt calls do set errno as it IS thread local.


 */
#include "callsystem.h"


/*
 there is actually no way to tell if the forked process failed because of some
 callsystem trouble or because the called programm signaled an nonzero exit code,
 all we can do is to send an informative error message to stderr.
 In case of callsystem trouble the exitcode is always 127 *AND* a message formatted as
 with "CALLSYSTEM_ERROR: message : errnumber : error_as_string" is send to stderr.

 This is only done on unix so far

 Since there is no "child" on windows error output(in debug compiles) go to the
 debbuger window. Release compiles the errors are silent.
 */
#ifdef unix
#define CALLSYSTEM_CHILD_ERROR(m)                                               \
do{                                                                            \
fprintf(stderr,                                                              \
"\nCALLSYSTEM_ERROR: %s : %d : %s\n", m, errno, strerror(errno));    \
exit(127);                                                                   \
} while(0)
#elif defined(WIN32)

#ifdef DEBUG
#define CALLSYSTEM_CHILD_ERROR(m) OutputDebugString(m) ;\
OutputDebugString("\n");
#else
#define CALLSYSTEM_CHILD_ERROR(m)
#endif

#endif



/*
 static declarations
 */
static int veczsize(char ** vec[]);
static size_t reserve_string(char ** s, size_t actual, size_t needed);

static char * alloc_executable_name(char ** env[], char ** argv[], const char * cmd);

#ifdef unix
static int setup_fd(callsystem_fd_t fds[2], callsystem_fd_t stdfd, int dir);
#elif defined(WIN32)
#include <sys/stat.h>
#include <fcntl.h>

/* quell some MS compiler warning nonsense. */
#define strdup _strdup
#define fdopen _fdopen

#define R_OK 04

static char* alloc_envblock(char * ptrs[]);
static char* alloc_commandline(char * ptrs[]);
#endif


/*
 int
 callsystem(const char * cmd,
 const char * argv[],
 const char * env[],
 callsystem_fd_t in[2],
 callsystem_fd_t out)[2],
 callsystem_fd_t err[2],
 const char * wd,
 int pri,
 callsystem_pid_t * child)

 parameters:
 cmd          command to execute
 argv         zero terminated array of options(only options,
 without the usual argv[0] == cmd, callsystem handles that internally)
 envp         zero terminated array of environment vars
(*in)[2]     fds to be connected to the childs stdin
(*out)[2]    fds to be connected to the childs stdout
(*err)[2]    fds to be connected to the childs stderr
 wd           working directory for the child process
 pri          priority advise 0=normal, 1=higher, -1=lower
 *child       where to store the childs pid, must be initialized to CALLSYSTEM_ILG_PID

 returns -1 when forking the child failed

 the fd's, argv, env and wd might be NULL
 envp will use defaults if NULL(see callsystem_exportdefaults() below)
 fd's will be bound to the standard streams when NULL
 working dir wont be changed if NULL
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
			callsystem_pid_t * const child)
{
#ifdef unix
	int ppri = getpriority(PRIO_PROCESS, 0);
	callsystem_pid_t cpid;

	if(*child != CALLSYSTEM_ILG_PID)
	{
		errno = EBUSY;
		return -1;
	}

	cpid = fork();
	if(cpid == -1)
		return -1;

	if(cpid == 0){
		/* child goes here */
		const char* bin;

		if( setup_fd(in, STDIN_FILENO, 0) == -1)
			CALLSYSTEM_CHILD_ERROR("illegal stdin");

		if( setup_fd(out, STDOUT_FILENO, 1) == -1)
			CALLSYSTEM_CHILD_ERROR("illegal stdout");

		if( setup_fd(err, STDERR_FILENO, 1) == -1)
			CALLSYSTEM_CHILD_ERROR("illegal stderr");

		if(wd && chdir(wd))
			CALLSYSTEM_CHILD_ERROR("illegal working directory");

		if(!env && callsystem_exportdefaults(&env))
			CALLSYSTEM_CHILD_ERROR("callsystem_exportdefaults failed");

		bin = alloc_executable_name(&env, &argv, cmd);

		/* put command name into argv[0]*/
		callsystem_argv_pushfront(&argv, strrchr(cmd, '/')?(strrchr(cmd, '/') + 1):cmd);

		if(pri != 0){
			setpriority(PRIO_PROCESS, 0, ppri +(pri>0?-5:5));
			errno = 0;
		}

		execve(bin,argv,env);
		CALLSYSTEM_CHILD_ERROR("execve failed");
	}
	/* parent */
	/* close fd's */
	if(in)
	{
		close(in[0]);
		in[0] = CALLSYSTEM_ILG_FD;
	}
	if(out)
	{
		close(out[1]);
		out[1] = CALLSYSTEM_ILG_FD;
	}
	if(err)
	{
		close(err[1]);
		err[1] = CALLSYSTEM_ILG_FD;
	}

	*child = cpid;

	return 0;
#elif defined(WIN32)

	char * bin;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	LPVOID argvblock;
	LPVOID envblock;

	/* we need to copy argv as we're not going to fork on windows. */
	char **argv_child;

	callsystem_argv_dup(argv, &argv_child);

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	si.dwFlags = STARTF_USESTDHANDLES;

	if(!in)
		si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	else
	{
		si.hStdInput = in[0];
		/* make sure the child process doesn't get the other
		 end of the pipe. */
		SetHandleInformation( in[1], HANDLE_FLAG_INHERIT, 0);
	}

	if(!out)
		si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	else
	{
		si.hStdOutput = out[1];
		SetHandleInformation( out[0], HANDLE_FLAG_INHERIT, 0);
	}

	if(!err)
		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	else
	{
		si.hStdError = err[1];
		SetHandleInformation( err[0], HANDLE_FLAG_INHERIT, 0);
	}

	/*environment*/
	if(!env && callsystem_exportdefaults(&env))
	{
		CALLSYSTEM_CHILD_ERROR("callsystem_exportdefaults failed");
		return -1;
	}

	bin = alloc_executable_name(&env, &argv_child, cmd);
	if(!bin)
	{
		CALLSYSTEM_CHILD_ERROR("command not found");
		return -1;
	}

	/*appname
	 */
	callsystem_argv_pushfront(&argv_child, cmd);

	/*commandline*/
	argvblock = alloc_commandline(argv_child);

	/* don't need this anymore. */
	callsystem_argv_clear(&argv_child);

	if(!argvblock)
		return -1;

	envblock = alloc_envblock(env);
	if(!envblock)
		return -1;

	/*
	 * You must have "inherit==TRUE" or the stdin/out/err handles
	 * won't be available to the sub-process.
	 */
	if(!CreateProcess(bin, argvblock, NULL, NULL, TRUE, 0, envblock, wd, &si, &pi ))
	{
		free(bin);
		return -1;
	}

	free(bin);

	/*
	 * You must close the "other" end of pipes.
	 * reading will just block if you don't
	 */
	if(out)
	{
		if(!CloseHandle(out[1]))
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return -1;
		}
		out[1] = CALLSYSTEM_ILG_FD;
	}

	if(err)
	{
		if(!CloseHandle(err[1]))
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return -1;
		}
		err[1] = CALLSYSTEM_ILG_FD;
	}


	(void) SetPriorityClass(pi.hProcess,
							pri<0?IDLE_PRIORITY_CLASS:
							pri>0?HIGH_PRIORITY_CLASS:NORMAL_PRIORITY_CLASS);

	CloseHandle(pi.hThread);
	*child = pi.hProcess;

	return 0;

#endif
}

/*
 int
 callsystem_running(callsystem_pid_t * pid);

 return >255 while the child is running,
 else its exit code
 return -1 in case of an error
 */
int
callsystem_running(callsystem_pid_t * pid)
{
#ifdef unix
	int status;

	if(*pid == CALLSYSTEM_ILG_PID)
	{
		errno = ECHILD;
		return -1;
	}

redo:
	if(waitpid(*pid, &status, WNOHANG) == -1)
	{
		if(errno == EINTR)
		{
			errno = 0;
			goto redo;
		}
		return -1;
	}

	if(!WIFEXITED(status))
		return 256;

	*pid = CALLSYSTEM_ILG_PID;
	return WEXITSTATUS(status);
#elif defined(WIN32)
	DWORD exitcode;

	if(*pid == CALLSYSTEM_ILG_PID)
		return -1;

	if(GetExitCodeProcess(*pid, &exitcode))
	{
		if(exitcode == STILL_ACTIVE)
			return 256;
	}
	else
		return -1;

	CloseHandle(*pid);
	*pid = CALLSYSTEM_ILG_PID;

	return exitcode;
#endif
}

/*
 int
 callsystem_finished(callsystem_pid_t * pid);

 return -1 in case of error, else waits for the child
 finished and return its exit code
 */
int callsystem_finished(callsystem_pid_t * pid)
{
#ifdef unix
	int status;

	if(*pid == CALLSYSTEM_ILG_PID)
	{
		errno = ECHILD;
		return -1;
	}

redo:
	if(waitpid(*pid, &status, 0) == -1)
	{
		if((errno = EINTR))
		{
			errno = 0;
			goto redo;
		}
		return -1;
	}

	*pid = CALLSYSTEM_ILG_PID;
	return WEXITSTATUS(status);
#elif defined(WIN32)
	DWORD exitcode;

	WaitForSingleObject(*pid , INFINITE);

	(void) GetExitCodeProcess(*pid, &exitcode);
	CloseHandle(*pid);
	*pid = CALLSYSTEM_ILG_PID;

	return exitcode;
#endif
}

/*
 int
 callsystem_abort(callsystem_pid_t * );

 kill the child unconditionally(cant be handled or ignored)
 you still have to call callsystem_finished(or callsystem_running)
 for the child, but can't trust its return value.
 */
int
callsystem_abort(callsystem_pid_t * pid)
{
#ifdef unix
	if(kill(*pid, SIGKILL) == -1)
		return -1;
	return 0;
#elif defined(WIN32)
	if(!TerminateProcess(*pid, 127))
		return -1;
	return 0;
#else
	return -1;
#endif
}


/*
 fd's are always used as pairs(two element array)
 [0] is the used for reading
 [1] is used for writing

 in case of a pipe this fd's represent the sides of the pipe and callsystem()
 will close the proper sides automatically
 in case of other kinds of descriptors, either side will be CALLSYSTEM_ILG_FD and must not be accessed
 */


/*
 int
 callsystem_pipe(callsystem_fd_t pipes[2]);

 sets up a pipe
 returns -1 on error and sets errno
 */
int
callsystem_pipe(callsystem_fd_t pipes[2])
{
#ifdef unix
	return pipe(pipes);
#elif defined(WIN32)
	HANDLE readh, writeh;
	/* Set the bInheritHandle flag so pipe handles are inherited. */
	SECURITY_ATTRIBUTES saAttr;

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	/* we don't know what this pipe will be used for so someone
	 else will have to make one end "un-inheritable" so the
	 child doesn't receive it. */
	if(CreatePipe(&(pipes[0]), &(pipes[1]), &saAttr, 0 /* buf size default*/))
		return 0;
	else
		return -1;
#endif
}


/*
 int
 callsystem_null(callsystem_fd_t null[2]);

 portably opens a null device
 returns -1 on error and sets errno
 */
int
callsystem_null(callsystem_fd_t null[2])
{
#ifdef unix
	int fd = open("/dev/null",O_RDWR);
#elif defined(WIN32)
	HANDLE fd = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
						   FILE_SHARE_READ | FILE_SHARE_WRITE,
						   0, OPEN_EXISTING, 0, NULL);
#endif
	if(fd == CALLSYSTEM_ILG_FD)
		return -1;

	null[0] = null[1] = fd;
	return 0;
}

/*
 int
 callsystem_open(const char * filename, enum callsystem_filemode mode,callsystem_fd_t(*fd)[2])

 open a file

 where mode is one of
 CALLSYSTEM_MODE_READ         - open *existing* file for reading
 CALLSYSTEM_MODE_WRITE        - not used here(dont use it)
 CALLSYSTEM_MODE_CREATE       - open *non existing* file for writing
 CALLSYSTEM_MODE_APPEND       - open *existing* file for writing at the end
 CALLSYSTEM_MODE_OVERWRITE    - open or create file for overwriting

 possibly or'ed with
 CALLSYSTEM_MODE_BINARY       - not evaluated here(but permitted)

 returns 0 on success, else -1 and sets errno
 */
int
callsystem_open(const char * filename,
				enum callsystem_filemode mode,
				callsystem_fd_t fd[2])
{
#ifdef unix
	switch(mode & ~CALLSYSTEM_MODE_BINARY)
	{
		case CALLSYSTEM_MODE_READ:
			fd[1] = CALLSYSTEM_ILG_FD;
			fd[0] = open(filename, O_RDONLY);
			if(fd[0] == -1)
				return -1;
			break;
			case CALLSYSTEM_MODE_CREATE:
			fd[0] = CALLSYSTEM_ILG_FD;
			fd[1] = open(filename, O_WRONLY|O_CREAT|O_EXCL, 0774);
			if(fd[1] == -1)
				return -1;
			break;
			case CALLSYSTEM_MODE_APPEND:
			fd[0] = CALLSYSTEM_ILG_FD;
			fd[1] = open(filename, O_WRONLY|O_APPEND);
			if(fd[1] == -1)
				return -1;
			break;
			case CALLSYSTEM_MODE_OVERWRITE:
			fd[0] = CALLSYSTEM_ILG_FD;
			fd[1] = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0774);
			if(fd[1] == -1)
				return -1;
			break;
			default:
			errno = EINVAL;
			return -1;
	}
#elif defined(WIN32)
	/*TODO*/
	(void) filename;
	(void) mode;
	(void) fd;
#endif
	return 0;
}

/*
 FILE *
 callsystem_fdopen(callsystem_fd_t * , enum callsystem_filemode mode);

 create a std C FILE stream from a underlying filedescriptor pair.

 fds   - filehandle pair
 mode  - the mode used in callsystem_open or
 CALLSYSTEM_MODE_READ
 CALLSYSTEM_MODE_WRITE
 for pipes

 possibly or'ed with
 CALLSYSTEM_MODE_BINARY       - open the file in binary mode

 */
FILE *
callsystem_fdopen(callsystem_fd_t fds[2], enum callsystem_filemode mode)
{
#ifdef unix
	switch(mode & ~CALLSYSTEM_MODE_BINARY)
	{
		case CALLSYSTEM_MODE_READ:
			return fdopen(fds[0], "r");
		case CALLSYSTEM_MODE_WRITE:
			return fdopen(fds[1], "w");
		case CALLSYSTEM_MODE_CREATE:
			return fdopen(fds[1], "w+");
		case CALLSYSTEM_MODE_APPEND:
			return fdopen(fds[1], "a");
		case CALLSYSTEM_MODE_OVERWRITE:
			return fdopen(fds[1], "w+");
		default:
			errno = EINVAL;
			return NULL;
	}
#elif defined(WIN32)

	int which;
	int osf_mode = 0;
	int fd;

	if(mode & CALLSYSTEM_MODE_BINARY)
		osf_mode = 0;
	else
		osf_mode = _O_TEXT;

	if((mode & ~CALLSYSTEM_MODE_BINARY) == CALLSYSTEM_MODE_READ)
		fd = _open_osfhandle(fds[0], osf_mode | _O_RDONLY);
	else
		fd = _open_osfhandle(fds[0], osf_mode | _O_APPEND);
	if(fd == -1)
		return NULL;

	if(mode & CALLSYSTEM_MODE_BINARY)
		switch(mode & ~CALLSYSTEM_MODE_BINARY)
	{
		case CALLSYSTEM_MODE_READ:
			return fdopen(fd, "rb");
		case CALLSYSTEM_MODE_WRITE:
			return fdopen(fd, "wb");
		case CALLSYSTEM_MODE_CREATE:
			return fdopen(fd, "wb+");
		case CALLSYSTEM_MODE_APPEND:
			return fdopen(fd, "ab");
		case CALLSYSTEM_MODE_OVERWRITE:
			return fdopen(fd, "wb+");
		default:
			errno = EINVAL;
			return NULL;
	}
	else
		switch(mode & ~CALLSYSTEM_MODE_BINARY)
	{
		case CALLSYSTEM_MODE_READ:
			return fdopen(fd, "r");
		case CALLSYSTEM_MODE_WRITE:
			return fdopen(fd, "w");
		case CALLSYSTEM_MODE_CREATE:
			return fdopen(fd, "w+");
		case CALLSYSTEM_MODE_APPEND:
			return fdopen(fd, "a");
		case CALLSYSTEM_MODE_OVERWRITE:
			return fdopen(fd, "w+");
		default:
			errno = EINVAL;
			return NULL;
	}

#elif 0
	switch(mode)
	{
			/*| CALLSYSTEM_MODE_BINARY */
		case CALLSYSTEM_MODE_READ:
			return fdopen(fds[0], "r");
		case CALLSYSTEM_MODE_WRITE:
			return fdopen(fds[1], "w");
		case CALLSYSTEM_MODE_CREATE:
			return fdopen(fds[1], "w+");
		case CALLSYSTEM_MODE_APPEND:
			return fdopen(fds[1], "a");
		case CALLSYSTEM_MODE_OVERWRITE:
			return fdopen(fds[1], "w+");
		default:
			errno = EINVAL;
			return NULL;
	}
#endif
}

/*
 int
 callsystem_close(callsystem_fds_t fds[2])

 close a file descriptor pair
 */
int
callsystem_close(callsystem_fd_t fds[2])
{
#ifdef unix
	int r=0;

	if(fds[0] == fds[1] && fds[0] != CALLSYSTEM_ILG_FD)
	{
		r += close(fds[0]);
	}
	else
	{
		if(fds[0] != CALLSYSTEM_ILG_FD)
			r += close(fds[0]);

		if(fds[1] != CALLSYSTEM_ILG_FD)
			r += close(fds[1]);
	}
	fds[0] = fds[1] = CALLSYSTEM_ILG_FD;

	if(r)
		return -1;
	else
		return 0;
#elif defined(WIN32)
	int r = 0;
	if(fds[0] == fds[1] && fds[0] != CALLSYSTEM_ILG_FD)
	{
		if(!CloseHandle(fds[0]))
			r++;
	}
	else
	{
		if(fds[0] != CALLSYSTEM_ILG_FD)
			if(!CloseHandle(fds[0]))
				r++;
		if(fds[1] != CALLSYSTEM_ILG_FD)
			if(!CloseHandle(fds[1]))
				r++;
	}
	fds[0] = fds[1] = CALLSYSTEM_ILG_FD;
	if(r)
		return -1;
	else
		return 0;
#endif
}



/*
 ARGV HANDLING

 These function take a pointer to a argv which must be initialized to NULL and allocate
 all memory which is needed. Strings are duped.

 An argv might be used for serveral callsystem invocations and must be freed after use.
 */

/*
 int
 callsystem_argv_pushback(char ** argv[], const char * const arg)

 push an arg onto the back of the argv
 */
int
callsystem_argv_pushback(char ** argv[], const char * const arg)
{
	void * tmp;
	size_t sz;

	if(!*argv)
	{
		*argv = malloc(sizeof(char*));
		if(!*argv)
			return -1;
		*argv[0] = NULL;
	}

	sz = veczsize(argv);

	tmp = realloc(*argv,(1 + sz) * sizeof(char*));
	if(!tmp)
		return -1;

	*argv = tmp;

	tmp = strdup(arg);
	if(!tmp)
		return -1;
	(*argv)[sz - 1] = tmp;
	(*argv)[sz] = NULL;

	return 0;
}

/*
 int
 callsystem_argv_pushfront(char ** argv[], const char * const arg)

 push an arg onto the front of the argv
 */
int
callsystem_argv_pushfront(char ** argv[], const char * const arg)
{
	void * tmp;
	size_t sz;

	if(!*argv)
	{
		*argv = malloc(sizeof(char*));
		if(!*argv)
			return -1;
		*argv[0] = NULL;
	}

	sz = veczsize(argv);

	tmp = realloc(*argv,(1 + sz) * sizeof(char*));
	if(!tmp)
		return -1;

	*argv = tmp;

	(void) memmove(*argv + 1, *argv, sz * sizeof(char*));

	tmp = strdup(arg);
	if(!tmp)
		return -1;
	(*argv)[0] = tmp;

	return 0;
}

/*
 free the argvector
 */
int
callsystem_argv_dup(const char * argv[], char **argv_dup[])
{
	size_t sz;
	int i;

	if(!argv)
	{
		*argv_dup =(char**)0;
		return 0;
	}

	sz = veczsize((char***)&argv);
	*argv_dup = malloc(sz * sizeof(char*));

	if(!*argv_dup)
	{
		return -1;
	}

	for(i=0 ; i<sz ; i++)
	{
		if(argv[i])
		{
			(*argv_dup)[i] = strdup(argv[i]);
			if(!(*argv_dup)[i])
			{
				return -1;
			}
		}
		else
			(*argv_dup)[i] =(char*)0;
	}
	return 0;
}

/*
 free the argvector
 */
void
callsystem_argv_clear(char ** argv[])
{
	char ** i;

	if(!*argv)
		return;

	for(i=*argv;*i;++i)
		free(*i);

	free(*argv);
	*argv = NULL;
}


/*
 Setup of an environment for the child,
 this requires an initial char ** envp=NULL;
 such an environment must be freed after used(and might be reused in many callsystem calls)
 */

/*
 add an evnironment variable 'key' which holds 'value' to 'env'
 */
int
callsystem_setenv(char ** env[], const char * key, const char * value)
{
	void * tmp;
	size_t key_sz,sz;
	int envs;

	if(!*env)
	{
		*env = malloc(sizeof(char*));
		*env[0] = NULL;
	}
	if(!*env)
		return -1;

	sz =(key_sz = strlen(key)) + strlen(value) + 2;

	tmp = malloc(sz);
	if(!tmp)
		return -1;

	strcpy(tmp, key);
	((char*)tmp)[key_sz] = '=';
	strcpy(((char*)tmp)+key_sz+1, value);

	(void) callsystem_unsetenv(env, key);

	envs = veczsize(env);
	(*env)[envs-1] = tmp;

	tmp = realloc(*env,(envs+1) * sizeof(char*));
	if(!tmp)
	{
		free((*env)[envs]);
		(*env)[envs] = NULL;
		return -1;
	}

	*env = tmp;
	(*env)[envs] = NULL;

	return 0;
}


/*
 return the value of environment variable 'key' from 'envp'
 */
const char *
callsystem_getenv(char ** env[], const  char * key)
{
	char ** i;
	size_t sz;

	sz = strlen(key);

	if(!env || !*env)
		return NULL;

	for(i = *env; *i; ++i)
		if(!strncmp(key,*i,sz) &&(*i)[sz] == '=')
			return &(*i)[sz+1];

	return NULL;
}

/*
 int
 callsystem_unsetenv(char ** env[], const char * key)
 remove an environment variable 'key' from 'env'
 return 0 when successfully deleted
 return -1 in case 'key' was not part of the environment
 */
int
callsystem_unsetenv(char ** env[], const char * key)
{
	void * tmp;
	char ** i;
	size_t sz = strlen(key);

	if(!*env)
		return -1;

	for(i = *env; *i; ++i)
		if(!strncmp(key,*i,sz) &&(*i)[sz] == '=')
		{
			free(*i);
			break;
		}

	if(!*i)
		return -1;

	while((*i = *(i+1)))
		++i;

	tmp = realloc(*env,(i - *env + 1) * sizeof(char*) );
	if(tmp)
		*env = tmp;

	return 0;
}

/*
 int
 callsystem_exportenv(char ** env[], const char * key)

 export a environment variable 'key' from the current environment to 'env'

 returns -1 on error
 */
int
callsystem_exportenv(char ** env[], const char * key)
{
	const char * val;

	val = getenv(key);
	if(!val)
	{
		errno = EINVAL;
		return -1;
	}

	return callsystem_setenv(env, key, val);
}

/*
 int
 callsystem_exportdefaults(char** env[])

 export the environment variables which are expected to be available on the current system

 return -1 on allocation error
 */
int
callsystem_exportdefaults(char** env[])
{
	static const char * callsystem_default_envvars[]={
		"USER", "LOGNAME", "HOME", "LANG", "LC_ALL", "LC_COLLATE", "LC_CTYPE",
		"LC_MESSAGES", "LC_MONETARY", "LC_NUMERIC", "LC_TIME", "PATH", "MANPATH",
		"INFOPATH", "CDPATH", "DISPLAY", "PWD", "SHELL", "TERM", "PAGER", "EDITOR",
		"VISUAL", "BROWSER", "NLSPATH", "LOCPATH", "TMPDIR",
		"HOSTALIASES", "TZ", "TZDIR", "TERMCAP",
		"COLUMNS", "LINES",
#ifdef unix
		"LD_LIBRARY_PATH", "LD_PRELOAD", "POSIXLY_CORRECT",
		/* please review this list, I just added anything I found, naturally
		 * here shall only be the names which are expected for sane operation */
#elif defined(WIN32)
		/* */
#endif
		0,
	};

	const char ** i;
	for( i =  callsystem_default_envvars;*i;++i)
	{
		if(callsystem_exportenv(env, *i) == -1 && errno == ENOMEM)
			return -1;
	}
	return 0;
}

/*
 char *
 callsystem_env_subst(char ** envp[], const char * str);

 substitute environment variables from 'env' in 'str'
 return a freshly allocated string which must be freed after use

 environment variables must have the syntax ${KEY}, where KEY is the name of the variable
 A sole $ must be escaped by $$

 returns NULL on error and sets errno
 ENOMEM in case memory exhausted
 EINVAL in case of a syntax error, env var did not exist in environment

 Example:(environment FOO=foo BAR=bar)
 "${FOO}$$${BAR}"
 expands to:
 "foo$bar"
 */
char *
callsystem_env_subst(char ** env[], const char * str)
{
	char * ra;
	char * ret = NULL;
	size_t csr = 0;
	size_t sz = 0;

	for(; *str; ++str)
	{
		if(*str == '$')
		{
			++str;
			if(*str == '{')
			{
				/* ${ */
				char * k;
				const char * v;
				size_t vs;
				const char * close;

				close = strchr(str + 1, '}');
				if(!close)
					goto einval;

				k = malloc(close - str);
				if(!k)
					goto emem;
				(void) strncpy(k, str + 1, close - str - 1);
				k[close - str - 1] = '\0';

				str = close;
				v = callsystem_getenv(env, k);
				free(k);

				if(!v)
				/* env var not found */
					goto einval;

				sz = reserve_string(&ret, sz, csr +(vs = strlen(v)) + 2);
				if(!sz)
					goto emem;

				(void) strcpy(&ret[csr], v);
				csr += vs;
			}
			else if(*str == '$')
				goto copy_char;
			else
				goto einval;
		}
		else
		{
		copy_char:
			sz = reserve_string(&ret, sz, csr + 1);
			if(!sz)
				goto emem;
			ret[csr++] = *str;
		}
	}

	++csr;
	sz = reserve_string(&ret, sz, csr + 1);
	if(!sz)
		goto emem;
	ret[csr] = '\0';

	/* try to shrink the return space */
	ra = realloc(ret, sz);
	if(ra)
		ret = ra;

	return ret;

einval:
	/* syntax error*/
	errno = EINVAL;
emem:
	/* key allocation error */
	free(ret);
	return 0;
}


/*
 free all resources used by the environment 'env'
 */
void
callsystem_env_clear(char ** env[])
{
	char ** i;

	if(!*env)
		return;

	for(i=*env;*i;++i)
		free(*i);

	free(*env);
	*env = NULL;
}



/*
 char *
 callsystem_path_to_host(const char*)

 char *
 callsystem_path_to_unix(const char *)

 convert unix like filenames to OS specific filenames(and back)

 for win32:
 "/Letter:/" becomes "Letter:\"
 "/foo/" becomes "\foo\"

 returns a freshly allocated string or NULL on error
 must be freed after use
 */
char *
callsystem_path_to_host(const char * str)
{
#ifdef unix
	return strdup(str);
#elif defined(WIN32)
	/*TODO*/
	(void) str;
	return NULL;
#endif
}

char *
callsystem_path_to_unix(const char * str)
{
#ifdef unix
	return strdup(str);
#elif defined(WIN32)
	/*TODO*/
	(void) str;
	return NULL;
#endif
}



/*
 STATIC TOOL FUNCTIONS
 */

/*
 used internally, dup2 one fd and close the other of a fd pair
 */
#ifdef unix
static int
setup_fd(callsystem_fd_t fds[2], callsystem_fd_t stdfd, int dir)
{
	int flags;

	dir &= 1; /* only 0 an 1 are permitted */

	if(fds)
	{
		if(fds[dir] == CALLSYSTEM_ILG_FD)
			return -1;

	retry:
		if(dup2(fds[dir], stdfd) == -1)
		{
			if(errno == EINTR)
				goto retry;
			else
				return -1;
		}

		if((flags = fcntl(stdfd, F_GETFD, 0)) < 0)
			return -1;
		if(fcntl(stdfd, F_SETFD, flags & ~FD_CLOEXEC) < 0)
			return -1;

		close(fds[dir]);
		close(fds[!dir]);
		fds[!dir] = fds[dir] = CALLSYSTEM_ILG_FD;
	}
	return 0;
}
#endif


/*
 lookup the cmd in path, check access, check #! on windows and so on
 */
#ifdef unix
static char *
alloc_executable_name(char ** env[], char ** argv[], const char * cmd)
{
	struct stat s;
	char * bin = NULL;

	(void) argv; /* not needed for unix, Win32 will need it implement #!/bin/... */

	if(strchr(cmd,'/') != NULL)
	{
		/* cmd includes path to binary */
		bin = strdup(cmd);
		if(!bin)
			CALLSYSTEM_CHILD_ERROR("strdup");

		if(stat(bin,&s) < 0 || !S_ISREG(s.st_mode))
			goto cmd_not_useable;
	}
	else
	{
		/* search cmd in $PATH */
		const char * path;
		char * path_dup;
		char * csr;
		char * pos;
		size_t csr_len;

		path = callsystem_getenv(env, "PATH");
		if(!path)
			CALLSYSTEM_CHILD_ERROR("PATH not set");

		path_dup = strdup(path);
		if(!path_dup)
			CALLSYSTEM_CHILD_ERROR("strdup");

		for(csr = strtok_r(path_dup, ":", &pos); csr; csr = strtok_r(NULL, ":", &pos))
		{
			bin = malloc((csr_len = strlen(csr)) + strlen(cmd) + 2 );

			strcpy(bin,csr);
			bin[csr_len] = '/';
			strcpy(bin+csr_len+1, cmd);

			if(!stat(bin,&s) && S_ISREG(s.st_mode))
				break;

			free(bin);
			bin = NULL;
		}
		free(path_dup);
	}

	if(!bin)
		goto cmd_not_useable;

	if(access(bin, R_OK | X_OK) < 0)
		goto cmd_not_useable;

	return bin;
cmd_not_useable:
	CALLSYSTEM_CHILD_ERROR("cmd not executable");
}
#elif defined(WIN32)

/*
 * look for cmd on disk. If cmd is a "path" don't search
 * either way append the .exe if needed
 * path is a ; separated list of directories to look in
 */
char * find_in_path(const char* cmd, const char* path)
{
	struct stat s;
	int gotDotExe;
	const char * dotExe = ".exe";
	char * bin = NULL;
	/* we're willing to add .exe onto the end of a path. Maybe
	 we should be more windows friendly and consult COMSPEC?
	 we're not too worried about false positives here*/
	gotDotExe = strstr(cmd, dotExe) ? 1 : 0;

	/* if cmd is a path don't search */
	/* XXX this fails on c:program.exe which is a path */
	if(strchr(cmd,'/') != NULL || strchr(cmd, '\\') != NULL)
	{
		/* cmd includes path to binary */
		bin = malloc(strlen(cmd) + strlen(dotExe) + 1);
		if(!bin)
		{
			CALLSYSTEM_CHILD_ERROR("strdup");
			return NULL;
		}
		strcpy(bin, cmd);

		if(stat(bin,&s) < 0 || !(s.st_mode & _S_IFREG))
		{
			if(!gotDotExe)
			{
				strcat(bin, dotExe);
				if(stat(bin,&s) < 0 || !(s.st_mode & _S_IFREG))
				{
					free(bin);
					return NULL;
				}
			}
			else
			{
				free(bin);
				return NULL;
			}
		}
	}
	else
	{
		/* search cmd in path*/
		char * path_dup;
		char * csr;
		size_t csr_len;

		path_dup = strdup(path);
		if(!path_dup)
		{
			CALLSYSTEM_CHILD_ERROR("strdup");
			return NULL;
		}

		/*
		 * haha strtok is thread safe in the ms crt but path is delimeted by ';'!
		 */
		for(csr = strtok(path_dup, ";"); csr; csr = strtok(NULL, ";"))
		{
			bin = malloc((csr_len = strlen(csr)) + strlen(cmd) + 2 +
						  strlen(dotExe));

			strcpy(bin,csr);
			bin[csr_len] = '/';
			strcpy(bin+csr_len+1, cmd);

			if(!stat(bin,&s) &&(s.st_mode & _S_IFREG))
				break;

			if(!gotDotExe)
			{
				strcat(bin, dotExe);
				if(!stat(bin,&s) &&(s.st_mode & _S_IFREG))
					break;
			}

			free(bin);
			bin = NULL;
		}
		free(path_dup);
	}

	return bin;
}
/*for OSes which dont support #!interpreters*/

/*search cmd in PATH*/

/*if check for #!shebang*/
/*   search interpreter*/
/*   check executable interpreter*/
/*   check read cmd*/
/*   assemble argv*/
/*else*/
/*   check executable cmd*/
static char *
alloc_executable_name(char ** env[], char ** argv[], const char * cmd)
{
	char * bin = NULL;
	char * interp = NULL;
	char * interp_arg = NULL;
	const char * path;

	path = callsystem_getenv(env, "PATH");
	if(!path)
	{
		CALLSYSTEM_CHILD_ERROR("PATH not set");
		return NULL;
	}

	bin = find_in_path(cmd, path);

	if(!bin)
		goto cmd_not_useable;

	/* access can't check for execute permission on win32. */
	if(access(bin, R_OK) < 0)
		goto cmd_not_useable;

	/* on unix we'd be done but to support #! we now need to discover if we have
	 * a an intrepreter script. If we do then jigger the argv and bin */
	if(is_interpreter(bin, &interp, &interp_arg) == -1)
		goto cmd_not_useable;

	if(interp != NULL)
	{
		/* re-apply search to interpreter(might be missing .exe etc) */
		char *tmp = interp;
		interp = find_in_path(interp, path);
		free(tmp);

		if(!interp || access(interp, R_OK) < 0)
			goto cmd_not_useable;

		callsystem_argv_pushfront(argv, bin);
		free(bin);
		bin = interp;

		/* my BSD man page says one optional arg is allowed on the #! line */
		if(interp_arg != NULL)
		{
			callsystem_argv_pushfront(argv, bin);
			free(interp_arg);
		}
	}

	return bin;
cmd_not_useable:
	CALLSYSTEM_CHILD_ERROR("cmd not executable");
	return NULL;
}

/*
 open a file to see if its a #! script. return -1 if anything goes wrong
 XXX the chances someone would want a space in an interpreter path is pretty
 high on Windows. We completely fail in that case
 */
int is_interpreter(const char* bin, char ** interp, char ** interp_arg)
{
	char * line;
	char * get;
	FILE * fp;
	char *eol1;
	char *eol2;

	fp = fopen(bin, "rb");
	if(!fp)
		return -1;

	line = malloc(1024);
	if(line == NULL)
	{
		fclose(fp);
		return -1;
	}

	get = fgets(line, 256, fp);
	fclose(fp);

	if(get == NULL)
	{
		free(line);
		fclose(fp);
		return -1;
	}

	eol1 = strchr(line, '\n');
	eol2 = strchr(line, '\r');
	if(eol1) { *eol1 = 0; } // remove the \n return character
	if(eol2) { *eol2 = 0; } // remove the \r return character


	if(line[0] == '#' && line[1] == '!')
	{
		char *command, *arg;

		/* look for a command and one option arg like
		 my BSD man page says. */
		command = strtok(line, " \t");
		arg = strtok(NULL, " \t");

		/* sanity check */
		if(command[2] == '\0')
		{
			free(line);
			return -1;
		}

		/* skip over #! */
		command += 2;

		/* special case if we find /usr/bin/env */
		if(strcmp("/usr/bin/env", command) == 0)
		{
			if(arg == NULL)
				return -1;
			command = arg;
			arg = NULL;
		}
		*interp = strdup(command);

		if(*interp == NULL)
		{
			free(line);
			return -1;
		}

		if(arg != NULL)
		{
			*interp_arg = strdup(arg);
			if(*interp_arg == NULL)
			{
				free(line);
				free(*interp);
				*interp = NULL;
				return -1;
			}
		}
	}
	else
	{
		*interp = 0;
		*interp_arg = 0;
	}
	free(line);
	return 0;
}

#endif

/*
 count the members of a NULL terminated array of pointers
 */
static int
veczsize(char ** vec[])
{
	int i;
	char ** v = *vec;
	for(i=1;*v;++v,++i);
	return i;
}


/*
 realloc a string to at least needed size
 return the amount really reserved or 0 on error
 */
static size_t
reserve_string(char ** s, size_t actual, size_t needed)
{
	size_t n;
	char * r;

	for(n = actual>8?actual:8; n <= needed; n +=(n>>1)); /*n = n * 1.5*/

	r = realloc(*s, n);
	if(!r)
	{
		/* that was to much, try conservatively */
		r = realloc(*s, n = needed);
		if(!r)
			return 0;
	}
	*s = r;
	return n;
}

/*
 WIN32 specific helpers
 */
#ifdef WIN32
/*
 Win32 wants all env vars in a zero terminated block of C-Strings
 "foo=bar\0baz=barf\0\0" ...
 */
static char*
alloc_envblock(char * ptrs[])
{
	char ** csr;
	char * blk;
	char * blkcsr;
	size_t sz=0;

	if(!ptrs)
		return "";

	for(csr = ptrs; *csr; ++csr)
	{
		sz +=(strlen(*csr) + 1);
	}

	blk = malloc(sz + 1);
	if(!blk)
		return NULL;

	blkcsr = blk;

	for(csr = ptrs; *csr; ++csr)
	{
		(void) strcpy(blkcsr,*csr);
		blkcsr +=(strlen(*csr) + 1);
	}

	*blkcsr = '\0';
	return blk;
}

/*
 the commandline(argv) in Win32 is passed only as one string with
 proper double-quoting and escaping
 */
static char*
alloc_commandline(char * ptrs[])
{
	char ** csr;
	char * csr2;
	char * blk;
	char * blkcsr;
	size_t sz=0;

	if(!ptrs)
		return "";

	/* lets count the space needed*/
	for(csr = ptrs; *csr; ++csr)
	{
		sz +=(strlen(*csr) + 1);

		/* spaces in string? we need 2 quotes */
		if(strchr(*csr, ' '))
			sz += 2;

		/* " have to be escaped, \" needs one extra char */
		for(csr2 = *csr; *csr2 &&(csr2 = strchr(csr2, '"')); ++csr2)
			++sz;

		/* i do NOT handle other whitespaces here, if that is a problem, send me patches */
	}

	blk = malloc(sz);
	if(!blk)
		return NULL;

	blkcsr = blk;

	/* now copy, quote, escape, terminate, space ... */
	for(csr = ptrs; *csr; ++csr)
	{
		/* leading " when needed */
		if(strchr(*csr, ' '))
			*blkcsr++ = '"';

		/* copy the string */
		for(csr2 = *csr; *csr2; ++csr2)
		{
			/* escape for \" */
			if(*csr2 == '"')
				*blkcsr++ = '\\';
			*blkcsr++ = *csr2;
		}

		/* trailing " when needed */
		if(strchr(*csr, ' '))
			*blkcsr++ = '"';

		/* one space to the next argument */
		*blkcsr++ = ' ';
	}

	/* trailing \0 overwrites the last space */
	*(blkcsr - 1) = '\0';
	return blk;
}



#endif
/*TODO*/
/*
 fprintf(stderr,                                                              \
 "\nCALLSYSTEM_ERROR: %s : %d : %s\n", m, errno, strerror(errno));    \
 exit(127);                                                                   \

 */
/*
 void ErrorExit(LPTSTR lpszFunction)
 {
 TCHAR szBuf[80];
 LPVOID lpMsgBuf;
 DWORD dw = GetLastError();

 FormatMessage(
 FORMAT_MESSAGE_ALLOCATE_BUFFER |
 FORMAT_MESSAGE_FROM_SYSTEM,
 NULL,
 dw,
 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
(LPTSTR) &lpMsgBuf,
 0, NULL );

 wsprintf(szBuf,
 "%s failed with error %d: %s",
 lpszFunction, dw, lpMsgBuf);

 MessageBox(NULL, szBuf, "Error", MB_OK);

 LocalFree(lpMsgBuf);
 ExitProcess(dw);
 }

 */
