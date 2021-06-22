
// metadoc Directory category Core
// metadoc Directory copyright Steve Dekorte 2002
// metadoc Directory license BSD revised
/*metadoc Directory description
The Directory object supports accessing filesystem directories. A note on paths;
if a path begins with a "/" it's the root,
if it beings with a "./" it's the launch path,
if not specified, "./" is assumed.""")
*/
/*metadoc Directory credits
Cygwin code by Mike Austin. WIN32 code by Daniel Vollmer.
*/

#include "IoDirectory.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoFile.h"
#include <sys/stat.h>

#if !defined(_MSC_VER) && !defined(__SYMBIAN32__)
#include <unistd.h> /* ok, this isn't ANSI */
#endif

#if defined(_MSC_VER) && !defined(__SYMBIAN32__)
#include <direct.h>
#define getcwd _getcwd
#endif

#if defined(__SYMBIAN32__)
static char *getcwd(char *buf, int size) { return 0; }
#endif

#ifndef _WIN32

#include <dirent.h>
#include <sys/file.h>
#include <unistd.h>
#define MKDIR mkdir

#else

#include <windows.h>
#ifndef __MINGW64__
#define S_IRGRP 0
#define S_IXGRP 0
#define S_IROTH 0
#define S_IXOTH 0
#endif
#ifndef __MINGW32__
#define S_IRWXU 0
#endif

#define DT_UNKNOWN 0
#define DT_DIR 1
#define DT_REG 8
#define DT_LNK 10
#define MKDIR mkdir_win32

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

static DIR *opendir(char *pSpec) {
    DIR *pDir = io_calloc(1, sizeof *pDir);
    char *longer_string =
        io_calloc(1, (strlen(pSpec) + 3) * sizeof *longer_string);

    strcpy(longer_string, pSpec);
    strcat(longer_string, "/*");
    pDir->hFind = FindFirstFile(longer_string, &pDir->wfd);
    io_free(longer_string);
    pDir->valid = pDir->hFind != INVALID_HANDLE_VALUE;

    if (!pDir->valid) {
        DWORD err = GetLastError();
        if (err == ERROR_PATH_NOT_FOUND) {
            io_free(pDir);
            return (DIR *)0;
        }
    }

    return pDir;
}

static void closedir(DIR *pDir) {
    if (pDir->hFind != INVALID_HANDLE_VALUE) {
        FindClose(pDir->hFind);
    }

    io_free(pDir);
}

static struct dirent *readdir(DIR *pDir) {
    if (pDir->valid) {
        strcpy(pDir->de.d_name, pDir->wfd.cFileName);
        if (pDir->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            pDir->de.d_type = DT_DIR;
        } else if (pDir->wfd.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) {
            pDir->de.d_type = DT_REG;
        }
#ifdef FILE_ATTRIBUTE_REPARSE_POINT
        else if (pDir->wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            pDir->de.d_type = DT_LNK;
        }
#endif
        else {
            pDir->de.d_type = DT_UNKNOWN;
        }
        pDir->valid = FindNextFile(pDir->hFind, &pDir->wfd);
        return &pDir->de;
    }

    return NULL;
}

typedef int mode_t_win32;

int mkdir_win32(const char *path, mode_t_win32 mode) {
    /* returns zero on sucess */
    LPCTSTR lpPathName = path;
    LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
    return (CreateDirectory(lpPathName, lpSecurityAttributes) == 0);
}

/*
int chdir(const char *path)
{
        LPCTSTR lpPathName = path;
        return SetCurrentDirectory(lpPathName) ? 1 : -1;
}
*/
#endif

int isDirectory(struct dirent *dp, const char *path) {
    struct stat st;

#if (defined _DIRENT_HAVE_D_TYPE && defined _BSD_SOURCE) || defined _WIN32
    if (dp->d_type != DT_UNKNOWN) {
        // if it says so, let it be directory
        if (dp->d_type == DT_DIR) {
            return 1;
        }
        // if it's a symlink, need to stat to know what it points to
        if (dp->d_type != DT_LNK) {
            return 0;
        }
    }
#endif

    if (!stat(path, &st)) {
        return (st.st_mode & S_IFMT) == S_IFDIR;
    }

    // undefined behaviour here
    return 0;
}

static const char *protoId = "Directory";

#define DATA(self) ((IoDirectoryData *)IoObject_dataPointer(self))

IoTag *IoDirectory_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoDirectory_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoDirectory_free);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoDirectory_mark);
    return tag;
}

IoDirectory *IoDirectory_proto(void *state) {
    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoDirectory_newTag(state));

    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoDirectoryData)));
    DATA(self)->path = IOSYMBOL(".");

    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    {
        IoMethodTable methodTable[] = {
            {"setPath", IoDirectory_setPath},
            {"path", IoDirectory_path},
            {"name", IoDirectory_name},
            {"exists", IoDirectory_exists},
            {"items", IoDirectory_items},
            {"at", IoDirectory_at},
            {"size", IoDirectory_size},
            {"create", IoDirectory_create},
            {"createSubdirectory", IoDirectory_createSubdirectory},
            {"currentWorkingDirectory", IoDirectory_currentWorkingDirectory},
            {"setCurrentWorkingDirectory",
             IoDirectory_setCurrentWorkingDirectory},
            {NULL, NULL},
        };

        IoObject_addMethodTable_(self, methodTable);
    }
    return self;
}

IoDirectory *IoDirectory_rawClone(IoDirectory *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(
        self, cpalloc(IoObject_dataPointer(proto), sizeof(IoDirectoryData)));
    return self;
}

IoDirectory *IoDirectory_new(void *state) {
    IoDirectory *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

// -----------------------------------------------------------

IoDirectory *IoDirectory_newWithPath_(void *state, IoSymbol *path) {
    IoDirectory *self = IoDirectory_new(state);
    DATA(self)->path = IOREF(path);
    return self;
}

IoDirectory *IoDirectory_cloneWithPath_(IoDirectory *self, IoSymbol *path) {
    IoDirectory *d = IOCLONE(self);
    DATA(d)->path = IOREF(path);
    return d;
}

void IoDirectory_free(IoDirectory *self) {
    io_free(IoObject_dataPointer(self));
}

void IoDirectory_mark(IoDirectory *self) {
    IoObject_shouldMark((IoObject *)DATA(self)->path);
}

// -----------------------------------------------------------

IO_METHOD(IoDirectory, path) {
    /*doc Directory path
    Returns the directory path. The default path is '.'.
    */

    return DATA(self)->path;
}

IO_METHOD(IoDirectory, setPath) {
    /*doc Directory setPath(aString)
    Sets the directory path. Returns self.
    */

    DATA(self)->path = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));

    /*
    {
            UArray *path = IoSeq_rawUArray(DATA(self)->path);
            printf("IoDirectory_setPath path = \"%s\" %i\n", UTF8CSTRING(path),
    UArray_itemSize(path));
    }
    */
    return self;
}

IO_METHOD(IoDirectory, name) {
    /*doc Directory name
    Returns the receiver's last path component.
    */

    return IoSeq_lastPathComponent(DATA(self)->path, locals, m);
}

// _DARWIN_FEATURE_64_BIT_INODE

IoObject *IoDirectory_itemForDirent_(IoDirectory *self, struct dirent *dp) {
    IoSymbol *pathString;
    int isDir;
    UArray *path = IoSeq_rawUArray(DATA(self)->path);
    UArray *ba = UArray_clone(path);
    UArray_convertToUTF8(ba);

    if (UArray_size(ba) &&
        !IS_PATH_SEPERATOR(UArray_longAt_(ba, UArray_size(ba) - 1))) {
        UArray_appendCString_(ba, IO_PATH_SEPARATOR);
    }

    UArray_appendBytes_size_(ba, (const unsigned char *)(dp->d_name),
                             strlen(dp->d_name));

    // printf("ba1: '%s' %i\n", (char *)UArray_bytes(ba),
    // (int)UArray_sizeInBytes(ba));

    isDir = isDirectory(dp, (const char *)UArray_bytes(ba));

    pathString =
        IoState_symbolWithUArray_copy_convertToFixedWidth(IOSTATE, ba, 0);

    if (isDir) {
        return IoDirectory_newWithPath_(IOSTATE, pathString);
    }

    return IoFile_newWithPath_(IOSTATE, pathString);
}

IO_METHOD(IoDirectory, exists) {
    /*doc Directory exists(optionalPath)
    Returns true if the Directory path exists, and false otherwise.
    If optionalPath string is provided, it tests the existence of that path
    instead.
    */

    IoSymbol *path = DATA(self)->path;
    DIR *dirp;

    if (IoMessage_argCount(m) > 0) {
        path = IoMessage_locals_symbolArgAt_(m, locals, 0);
    }

#if !defined(_WIN32) || defined(__CYGWIN__)
    dirp = opendir(UTF8CSTRING(path));

    if (!dirp) {
        return IOFALSE(self);
    }

    (void)closedir(dirp);
    return IOTRUE(self);
#else
    {
        DWORD d = GetFileAttributes(UTF8CSTRING(path));
        return (d != INVALID_FILE_ATTRIBUTES) && (d & FILE_ATTRIBUTE_DIRECTORY)
                   ? IOTRUE(self)
                   : IOFALSE(self);
    }
#endif
}

IO_METHOD(IoDirectory, items) {
    /*doc Directory items
    Returns a list object containing File and Directory objects
    for the files and directories of the receiver's path.
    */

    IoList *items = IoList_new(IOSTATE);
    DIR *dirp = opendir(UTF8CSTRING(DATA(self)->path));
    struct dirent *dp;

    if (!dirp) {
        IoState_error_(IOSTATE, m, "Unable to open directory %s",
                       UTF8CSTRING(DATA(self)->path));
    }

    while ((dp = readdir(dirp)) != NULL) {
        IoList_rawAppend_(items, IoDirectory_itemForDirent_(self, dp));
    }

    (void)closedir(dirp);

    return items;
}

IoObject *IoDirectory_justFullPath(IoDirectory *self, IoSymbol *name) {
    UArray *fullPath = UArray_clone(IoSeq_rawUArray(DATA(self)->path));
    UArray_appendPath_(fullPath, IoSeq_rawUArray(name));
    return IoState_symbolWithUArray_copy_convertToFixedWidth(IOSTATE, fullPath,
                                                             0);
}

IoObject *IoDirectory_justAt(IoDirectory *self, IoSymbol *name) {
    IoState *state = IOSTATE;
    IoSymbol *fullPath = IoDirectory_justFullPath(self, name);
    struct stat st;

    if (stat(UTF8CSTRING(fullPath), &st) == -1) {
        return IONIL(self);
    }

    if ((st.st_mode & S_IFMT) == S_IFDIR) {
        return IoDirectory_newWithPath_(state, fullPath);
    } else {
        return IoFile_newWithPath_(state, fullPath);
    }

    return IONIL(self);
}

IO_METHOD(IoDirectory, at) {
    /*doc Directory at(aString)
    Returns a File or Directory object matching the name specified
    by aString or Nil if no such file or directory exists.
    */

    IoSymbol *name = IoMessage_locals_symbolArgAt_(m, locals, 0);
    return IoDirectory_justAt(self, name);
    /*
    IoObject *item = IoDirectory_justAt(self, name);
    if (ISNIL(item))
    {
            IoState_error_(IOSTATE, m, "Unable to open path %s",
    UTF8CSTRING(IoDirectory_justFullPath(self, name)));
    }
    return item;
    */
}

/*
IO_METHOD(IoDirectory, atPut)
{
        IoSymbol *name = IoMessage_locals_symbolArgAt_(m, locals, 0);
        IoObject *item = IoDirectory_justAt(self, name);
        if (ISNIL(item))
        {
                IoState_error_(IOSTATE,  m, "Unable to open path %s",
UTF8CSTRING(IoDirectory_justFullPath(self, name)));
        }
        return item;
}
*/

/*
IO_METHOD(IoDirectory, itemNamed)
{
        IoSymbol *itemName = IoMessage_locals_symbolArgAt_(m, locals, 0);
        char *name = UTF8CSTRING(itemName);
        DIR *dirp = opendir(UTF8CSTRING(DATA(self)->path));
        struct dirent *dp;
        if (!dirp)
        {
                IoState_error_(IOSTATE, m, "Unable to open directory %s",
UTF8CSTRING(DATA(self)->path));
        }

        while ((dp = readdir(dirp)) != NULL)
        {
                if (strcmp(dp->d_name, name) == 0)
                {
                        IoObject *item = IoDirectory_itemForDirent_(self, dp);
                        (void)closedir(dirp);
                        return item;
                }
        }
        (void)closedir(dirp);
        return IONIL(self);
}
*/

IO_METHOD(IoDirectory, createSubdirectory) {
    /*doc Directory createSubdirectory(name)
    Create a subdirectory with the specified name.
    */

    IoState *state = IOSTATE;
    IoSymbol *subfolderName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject *currentItem = IoDirectory_justAt(self, subfolderName);

    if (ISDIRECTORY(currentItem)) {
        return currentItem;
    }

    if (ISFILE(currentItem)) {
        IoState_error_(IOSTATE, m,
                       "Attempt to create directory %s on top of existing file",
                       UTF8CSTRING(subfolderName));
    } else {
        IoSymbol *fullPath = IoDirectory_justFullPath(self, subfolderName);

        MKDIR(UTF8CSTRING(fullPath),
              S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        return IoDirectory_newWithPath_(state, fullPath);
    }

    return IONIL(self);
}

IO_METHOD(IoDirectory, create) {
    /*doc Directory create
    Create the directory if it doesn't exist.
    Returns self on success (or if the directory already exists), nil on
    failure.
    */

    if (!opendir(UTF8CSTRING(DATA(self)->path))) {
        // not there, so make it
        int r = MKDIR(UTF8CSTRING(DATA(self)->path),
                      S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        return (r == 0) ? self : IONIL(self);
    }

    return self;
}

IO_METHOD(IoDirectory, size) {
    /*doc Directory size
    Returns a Number containing the number of file and directory
    objects at the receiver's path.
    */

    int count = 0;
    DIR *dirp = opendir(UTF8CSTRING(DATA(self)->path));
    struct dirent *dp;

    if (!dirp) {
        IoState_error_(IOSTATE, m, "Unable to open directory %s",
                       UTF8CSTRING(DATA(self)->path));
    }

    while ((dp = readdir(dirp)) != NULL) {
        count++;
    }

    (void)closedir(dirp);
    return IONUMBER((double)count);
}

/* -------------------------------- */

UArray *IoDirectory_CurrentWorkingDirectoryAsUArray(void) {
#if defined(sparc) || defined(__sparc)
    char *buf = getcwd(NULL, FILENAME_MAX + 1);
#else
    char *buf = NULL;
    buf = (char *)getcwd(buf, 1024);
#endif /* sparc || _sparc */

    if (!buf) {
        return UArray_newWithCString_copy_(".", 1);
    } else {
        UArray *ba = UArray_newWithData_type_size_copy_(
            (unsigned char *)buf, CTYPE_uint8_t, strlen(buf), 1);
        UArray_setEncoding_(ba, CENCODING_UTF8);
        UArray_convertToFixedSizeType(ba);
        // io_free(buf); OSX get cwd man page says we should io_free this, but
        // MallocDebug does not like it
        return ba;
    }
}

int IoDirectory_SetCurrentWorkingDirectory(const char *path) {
    return chdir(path);
}

IO_METHOD(IoDirectory, currentWorkingDirectory) {
    /*doc Directory currentWorkingDirectory
    Returns the current working directory path.
    */

    return IoState_symbolWithUArray_copy_convertToFixedWidth(
        IOSTATE, IoDirectory_CurrentWorkingDirectoryAsUArray(), 0);
}

/*
int IoDirectory_SetCurrentWorkingDirectory(char *p)
{
        return chdir(p);
}
*/

IO_METHOD(IoDirectory, setCurrentWorkingDirectory) {
    /*doc Directory setCurrentWorkingDirectory(pathString)
    Sets the current working directory path.
    Returns true on success or false on error.
    */

    IoSymbol *path = IoMessage_locals_symbolArgAt_(m, locals, 0);

    if (chdir(UTF8CSTRING(path)) == -1) {
        return IOFAILURE(self);
    }

    return IOSUCCESS(self);
}
