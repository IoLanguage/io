

#include <stdio.h>

/* ----------------------------------------------------- */

#include <sys/stat.h>

#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>

/*#ifdef __CYGWIN__*/
#include <sys/file.h>
/*#endif*/
#else
#include <windows.h>
#define S_IRGRP 0
#define S_IXGRP 0
#define S_IROTH 0
#define S_IXOTH 0
#define S_IRWXU 0

#define DT_DIR 0x01

struct dirent {
	char d_name[MAX_PATH];
	unsigned char d_type;
};

typedef struct {
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	struct dirent de;
	unsigned char valid;
} DIR;

static DIR *opendir(char *pSpec)
{
	DIR *pDir = malloc(sizeof *pDir);
	char *longer_string = malloc((strlen(pSpec) + 3) * sizeof *longer_string);

	strcpy(longer_string, pSpec);
	strcat(longer_string, "/*");
	pDir->hFind = FindFirstFile(longer_string, &pDir->wfd);
	free(longer_string);
	pDir->valid = pDir->hFind != INVALID_HANDLE_VALUE;
	return pDir;
}

static void closedir(DIR * pDir)
{
	if (pDir->hFind != INVALID_HANDLE_VALUE)
	FindClose(pDir->hFind);
	free(pDir);
}

static struct dirent *readdir(DIR *pDir)
{
	if (pDir->valid)
	{
		strcpy(pDir->de.d_name, pDir->wfd.cFileName);
		pDir->de.d_type = (pDir->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ? DT_DIR : 0;
		pDir->valid = FindNextFile(pDir->hFind, &pDir->wfd);
		return &pDir->de;
	}
	return NULL;
}

typedef int mode_t;

int mkdir(const char *path, mode_t mode)
{
	/* returns zero on sucess */
	LPCTSTR lpPathName = path;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
	return (CreateDirectory(lpPathName, lpSecurityAttributes) != 0);
}

#endif

#if defined(__CYGWIN__) || defined(sun) || defined(__sun)
	int isDirectory(struct dirent *dp, char *path);
	{
		struct stat st;
		/*fstat( dp->d_fd, &st );*/
		stat(pathString, &st);
		return ( (st.st_mode & S_IFMT) == S_IFDIR );
	}
#else
	int isDirectory(struct dirent *dp, char *path)
	{
		return (dp->d_type == DT_DIR);
	}
#endif

/* ----------------------------------------------------- */

typedef struct
{
	char *path;
	DIR *dir;
	char *next;
} DirEnum;

DirEnum *DirEnum_new(void)
{
	DirEnum *self = calloc(1, sizeof(DirEnum));
	return self;
}

void DirEnum_free(DirEnum *self)
{
	if (self->path) free(self->path);
	free(self);
}

void DirEnum_setPath_(DirEnum *self, char *s)
{
	self->path = strcpy(realloc(self->path, strlen(s) + 1), s);
	self->dir = opendir(s);
}

char *DirEnum_setNext_(DirEnum *self, char *s)
{
	self->next = strcpy(realloc(self->next, strlen(s) + 1), s);
	return self->next;
}

char *DirEnum_next(DirEnum *self)
{
	struct direct *dp = readdir(dirp);
	if (dp)
	{
		char *name = dp.d_name;
	}
}

