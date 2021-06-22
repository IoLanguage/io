
// metadoc File copyright Steve Dekorte 2002
// metadoc File license BSD revised
// metadoc File credits Initial version contributed by Miles Egan.

#include "IoFile_stat.h"
#include "IoState.h"
#include "IoDate.h"
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DATA(self) ((IoFileData *)IoObject_dataPointer(self))

#ifndef lstat
#define lstat stat
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode) ((mode & _S_IFDIR) != 0)
#endif
#ifndef S_ISFIFO
#define S_ISFIFO(mode) ((mode & _S_IFIFO) != 0)
#endif
#ifndef S_ISREG
#define S_ISREG(mode) ((mode & _S_IFREG) != 0)
#endif
#ifndef S_ISLNK
#define S_ISLNK(mode) (0)
#endif
#ifndef S_ISSOCK
#define S_ISSOCK(mode) (0)
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE
#endif

void IoFile_statInit(IoFile *self) {
    {
        IoMethodTable methodTable[] = {
            {"stat", IoFile_stat},

            {"protectionMode", IoFile_protectionMode},
            {"lastAccessDate", IoFile_lastAccessDate},
            {"lastInfoChangeDate", IoFile_lastInfoChangeDate},
            {"lastDataChangeDate", IoFile_lastDataChangeDate},
            {"userId", IoFile_userId},
            {"groupId", IoFile_groupId},
            {"size", IoFile_statSize},

            {"isDirectory", IoFile_isDirectory},
            {"isPipe", IoFile_isPipe},
            {"isLink", IoFile_isLink},
            {"isRegularFile", IoFile_isRegularFile},
            {"isSocket", IoFile_isSocket},

            {"isUserExecutable", IoFile_isUserExecutable},
            {NULL, NULL},
        };
        IoObject_addMethodTable_(self, methodTable);
    }
}

struct stat *IoFile_statPointer(IoFile *self, IoObject *locals, IoMessage *m) {
    if (!DATA(self)->info) {
        IoFile_stat(self, locals, m);
    }

    return (struct stat *)DATA(self)->info;
}

IO_METHOD(IoFile, stat) {
    /*doc File stat
    Updates the receiver's meta info cache.
    */

    // struct stat *statInfo;

    if (!DATA(self)->info) {
        DATA(self)->info = io_calloc(1, sizeof(struct stat));
    }

    // statInfo = DATA(self)->info;

    if (stat(UTF8CSTRING(DATA(self)->path),
             (struct stat *)(DATA(self)->info)) != 0) {
        IoState_error_(IOSTATE, m, "unable to stat '%s': %s",
                       UTF8CSTRING(DATA(self)->path), strerror(errno));
    }

    return self;
}

/* ---------------------------------- */

IO_METHOD(IoFile, protectionMode) {
    /*doc File protectionMode
    Returns a Number containing the protection mode
    associated with the file's path.
    */

    return IONUMBER(IoFile_statPointer(self, locals, m)->st_mode);
}

#ifndef _POSIX_C_SOURCE
struct timeval timespec2timeval(struct timespec ts) {
    struct timeval tv;
    tv.tv_sec = ts.tv_sec;
    tv.tv_usec = ts.tv_nsec / 1000;
    return tv;
}
#endif

struct timeval time_t2timeval(time_t ts) {
    struct timeval tv;
    tv.tv_sec = ts;
    tv.tv_usec = 0;
    return tv;
}

IO_METHOD(IoFile, lastAccessDate) {
    /*doc File lastAccessDate
    Returns a Date object containing the last date and
    time the file was accessed.
    */

    struct stat *s = IoFile_statPointer(self, locals, m);
#ifndef _POSIX_C_SOURCE
    struct timeval tv = timespec2timeval(s->st_atimespec);
#else
    struct timeval tv = time_t2timeval(s->st_atime);
#endif

    return IoDate_newWithTimeval_(IOSTATE, tv);
}

IO_METHOD(IoFile, lastInfoChangeDate) {
    /*doc File lastInfoChangeDate
    Returns a Date object containing the last date and
    time the file's meta info was changed.
    */

    struct stat *s = IoFile_statPointer(self, locals, m);

#ifndef _POSIX_C_SOURCE
    struct timeval tv = timespec2timeval(s->st_ctimespec);
#else
    struct timeval tv = time_t2timeval(s->st_ctime);
#endif

    return IoDate_newWithTimeval_(IOSTATE, tv);
}

IO_METHOD(IoFile, lastDataChangeDate) {
    /*doc File lastDataChangeDate
    Returns a Date object containing the last date and
    time the file's contents were changed.
    */

    struct stat *s = IoFile_statPointer(self, locals, m);
#ifndef _POSIX_C_SOURCE
    struct timeval tv = timespec2timeval(s->st_mtimespec);
#else
    struct timeval tv = time_t2timeval(s->st_mtime);
#endif
    /*
    struct gettv get_tv;
    struct timezone timezone;
    struct tm *t;
    gettimeofday(&get_tv, &timezone);
    printf("stattv.tv_sec = %i\n", tv.tv_sec);
    printf("get_tv.tv_sec = %i\n", get_tv.tv_sec);
    */
    return IoDate_newWithTimeval_(IOSTATE, tv);
}

IO_METHOD(IoFile, userId) {
    /*doc File userId
    Returns a Number containing the user id associated with the file's path.
    */

    return IONUMBER(IoFile_statPointer(self, locals, m)->st_uid);
}

IO_METHOD(IoFile, groupId) {
    /*doc File groupId
    Returns a Number containing the group id associated with the file's path.
    */

    return IONUMBER(IoFile_statPointer(self, locals, m)->st_gid);
}

IO_METHOD(IoFile, statSize) {
    /*doc File statSize
    Returns the file's size in bytes as a Number.
    */

    return IONUMBER(IoFile_statPointer(self, locals, m)->st_size);
}

/* ---------------------------------- */

IO_METHOD(IoFile, isDirectory) {
    /*doc File isDirectory
    Returns true if the receiver's path points to a directory, false otherwise.
    */

    return IOBOOL(self, S_ISDIR(IoFile_statPointer(self, locals, m)->st_mode));
}

IO_METHOD(IoFile, isPipe) {
    /*doc File isPipe
    Returns true if the receiver is a pipe, false otherwise.
    */

    return IOBOOL(self, S_ISFIFO(IoFile_statPointer(self, locals, m)->st_mode));
}

IO_METHOD(IoFile, isLink) {
    /*doc File isLink
    Returns true if the receiver's path points to a link, false otherwise.
    */

    struct stat buf;

    if (lstat(UTF8CSTRING(DATA(self)->path), &buf) != 0) {
        IoState_error_(IOSTATE, m, "unable to stat '%s': %s",
                       UTF8CSTRING(DATA(self)->path), strerror(errno));
    }

    return IOBOOL(self, S_ISLNK((&buf)->st_mode));
}

IO_METHOD(IoFile, isRegularFile) {
    /*doc File isRegularFile
    Returns true if the receiver's file descriptor is a regular file, false
    otherwise.
    */

    return IOBOOL(self, S_ISREG(IoFile_statPointer(self, locals, m)->st_mode));
}

IO_METHOD(IoFile, isSocket) {
    /*doc File isSocket
    Returns true if the receiver's file descriptor is a Socket, false otherwise.
    */

    return IOBOOL(self, S_ISSOCK(IoFile_statPointer(self, locals, m)->st_mode));
}

IO_METHOD(IoFile, isUserExecutable) {
    /*doc File isUserExecutable
    Returns true if the receiver is user group executable, false otherwise.
    */

#ifdef ON_WINDOWS
    return IOFALSE(self);
#else
    mode_t mode = IoFile_statPointer(self, locals, m)->st_mode;
    mode_t check = S_IXUSR;
    return IOBOOL(self, mode & check);
#endif
}
