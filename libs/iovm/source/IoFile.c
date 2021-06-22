
// metadoc File copyright Steve Dekorte 2002
// metadoc File license BSD revised
// metadoc File category Core
/*metadoc File description
Encapsulates file i/o. Here's an example of opening a file,
and reversing its lines:
<pre>
file := File clone openForUpdating("/tmp/test")
lines := file readLines reverse
file rewind
lines foreach(line, file write(line, "\n"))
file close
</pre>
*/

#include "IoDate.h"
#include "IoFile.h"
#include "IoFile_stat.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoObject.h"
#include "IoList.h"
#include "IoSeq.h"
#include "UArray.h"
#include "PortableTruncate.h"
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
/*#include <sys/wait.h>*/

#if !defined(_MSC_VER) && !defined(__SYMBIAN32__)
#include <unistd.h> /* ok, this isn't ANSI */
#endif

#if defined(_MSC_VER) && !defined(__SYMBIAN32__)
#include <direct.h>
#define getcwd _getcwd
#define popen(x, y) _popen(x, y)
#define pclose(x) _pclose(x)
#endif

#if defined(__SYMBIAN32__)
static int pclose(void *f) { return 0; }
static int popen(void *f, int m) { return 0; }
static int rename(void *a, void *b) { return 0; }
static char *getcwd(char *buf, int size) { return 0; }
#endif

static const char *protoId = "File";

#define DATA(self) ((IoFileData *)IoObject_dataPointer(self))

int fileExists(char *path);

IoTag *IoFile_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_("File");
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoFile_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoFile_mark);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoFile_free);
    // IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc
    // *)IoFile_writeToStream_); IoTag_readFromStreamFunc_(tag,
    // (IoTagReadFromStreamFunc *)IoFile_readFromStream_);
    return tag;
}

IoFile *IoFile_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"descriptor", IoFile_descriptor},
        {"descriptorId", IoFile_descriptor}, // compatible with Socket
        // standard I/O
        {"standardInput", IoFile_standardInput},
        {"standardOutput", IoFile_standardOutput},
        {"standardError", IoFile_standardError},

        // path
        {"setPath", IoFile_setPath},
        {"path", IoFile_path},
        {"name", IoFile_lastPathComponent},
        {"temporaryFile", IoFile_temporaryFile},

        // info
        {"exists", IoFile_exists},
        {"size", IoFile_size},

        // open and close
        {"openForReading", IoFile_openForReading},
        {"openForUpdating", IoFile_openForUpdating},
        {"openForAppending", IoFile_openForAppending},
        {"mode", IoFile_mode},

        {"open", IoFile_open},
        {"reopen", IoFile_reopen},
        {"popen", IoFile_popen},
        {"close", IoFile_close},

        {"isOpen", IoFile_isOpen},

        // reading
        {"contents", IoFile_contents},
        {"asBuffer", IoFile_asBuffer},
        {"readLine", IoFile_readLine},
        {"readLines", IoFile_readLines},
        {"readStringOfLength", IoFile_readStringOfLength_},
        {"readBufferOfLength", IoFile_readBufferOfLength_},
        {"readToBufferLength", IoFile_readToBufferLength},
        {"at", IoFile_at},
        {"foreach", IoFile_foreach},
        {"foreachLine", IoFile_foreachLine},

        // writing
        {"write", IoFile_write},
        {"atPut", IoFile_atPut},
        {"flush", IoFile_flush},

        // positioning
        {"rewind", IoFile_rewind},
        {"setPosition", IoFile_position_},
        {"position", IoFile_position},
        {"positionAtEnd", IoFile_positionAtEnd},
        {"isAtEnd", IoFile_isAtEnd},

        // other
        {"remove", IoFile_remove},
        {"moveTo", IoFile_moveTo_},
        {"truncateToSize", IoFile_truncateToSize},
        /*
                {"makeUnbuffered", IoFile_makeUnbuffered},
                {"makeLineBuffered", IoFile_makeLineBuffered},
                {"makeFullyBuffered", IoFile_makeFullyBuffered},
        */
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoFile_newTag(state));

    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoFileData)));
    DATA(self)->path = IOSYMBOL("");
    DATA(self)->mode = IOSYMBOL("r+");
    DATA(self)->flags = IOFILE_FLAGS_NONE;
    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    IoFile_statInit(self);
    return self;
}

IoFile *IoFile_rawClone(IoFile *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(
        self, cpalloc(IoObject_dataPointer(proto), sizeof(IoFileData)));
    DATA(self)->info = NULL;
    DATA(self)->stream = (FILE *)NULL;
    DATA(self)->flags = IOFILE_FLAGS_NONE;
    return self;
}

IoFile *IoFile_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

IoFile *IoFile_newWithPath_(void *state, IoSymbol *path) {
    IoFile *self = IoFile_new(state);
    DATA(self)->path = IOREF(path);
    return self;
}

IoFile *IoFile_newWithStream_(void *state, FILE *stream) {
    IoFile *self = IoFile_new(state);
    DATA(self)->stream = stream;
    return self;
}

IoFile *IoFile_cloneWithPath_(IoFile *self, IoSymbol *path) {
    IoFile *f = IOCLONE(self);
    DATA(f)->path = IOREF(path);
    return f;
}

void IoFile_mark(IoFile *self) {
    IoObject_shouldMarkIfNonNull(DATA(self)->path);
    IoObject_shouldMarkIfNonNull(DATA(self)->mode);
}

void IoFile_free(IoFile *self) {
    if (NULL == IoObject_dataPointer(self)) {
        return;
    }

    IoFile_justClose(self);

    if (DATA(self)->info) {
        io_free(DATA(self)->info);
    }

    io_free(IoObject_dataPointer(self));
}

void IoFile_writeToStream_(IoFile *self, BStream *stream) {
    BStream_writeTaggedUArray_(stream, IoSeq_rawUArray(DATA(self)->path));
    BStream_writeTaggedUArray_(stream, IoSeq_rawUArray(DATA(self)->mode));
}

void *IoFile_readFromStream_(IoFile *self, BStream *stream) {
    IoSymbol *mode;
    IoSymbol *path = IoState_symbolWithUArray_copy_(
        IOSTATE, BStream_readTaggedUArray(stream), 1);
    DATA(self)->path = IOREF(path);
    mode = IoState_symbolWithUArray_copy_(IOSTATE,
                                          BStream_readTaggedUArray(stream), 1);
    DATA(self)->mode = IOREF(mode);
    return self;
}

void IoFile_justClose(IoFile *self) {
    FILE *stream = DATA(self)->stream;

    if (stream) {
        if (stream != stdout && stream != stdin) {
            if (DATA(self)->flags == IOFILE_FLAGS_PIPE) {
                int exitStatus = pclose(stream);
#if !defined(_MSC_VER) &&                                                      \
    !defined(__MINGW32__) /* No sys/wait.h in mingw, therefore can't use       \
                             WIFEXITED, WEXITSTATUS, etc. */
                if (WIFEXITED(exitStatus) == 1) {
                    exitStatus = WEXITSTATUS(exitStatus);
                    IoObject_setSlot_to_(self, IOSYMBOL("exitStatus"),
                                         IONUMBER(exitStatus));
                } else if (WIFSIGNALED(exitStatus) == 1) {
                    exitStatus = WTERMSIG(exitStatus);
                    IoObject_setSlot_to_(self, IOSYMBOL("termSignal"),
                                         IONUMBER(exitStatus));
                } else {
                    printf("Did not exit normally. Returned %d (%d)\n",
                           exitStatus, WEXITSTATUS(exitStatus));
                }
#else
                IoObject_setSlot_to_(self, IOSYMBOL("exitStatus"),
                                     IONUMBER(exitStatus));
#endif
            } else {
                fclose(stream);
                DATA(self)->flags = IOFILE_FLAGS_NONE;
            }
        }

        DATA(self)->stream = (FILE *)NULL;
    }
}

int fileExists(char *path) {
    struct stat statInfo;
    return stat(path, &statInfo) == 0;
}

int IoFile_justExists(IoFile *self) {
    return fileExists(UTF8CSTRING(DATA(self)->path));
}

int IoFile_create(IoFile *self) {
    FILE *fp = fopen(UTF8CSTRING(DATA(self)->path), "w");

    if (fp) {
        fclose(fp);
        return 1;
    }

    return 0;
}

/* ----------------------------------------------------------- */

IO_METHOD(IoFile, descriptor) {
    /*doc File descriptor
    Returns the file's descriptor as a number.
    */

    if (DATA(self)->stream) {
        return IONUMBER(fileno(DATA(self)->stream));
    } else {
        return IONIL(self);
    }
}

IO_METHOD(IoFile, standardInput) {
    /*doc File standardInput
    Returns a new File whose stream is set to the standard input stream.
    */

    IoFile *newFile = IoFile_new(IOSTATE);
    DATA(newFile)->path = IOREF(IOSYMBOL("<standard input>"));
    DATA(newFile)->mode = IOREF(IOSYMBOL("r"));
    DATA(newFile)->stream = stdin;
    DATA(newFile)->flags = IOFILE_FLAGS_NONE;
    return newFile;
}

IO_METHOD(IoFile, standardOutput) {
    /*doc File standardOutput
    Returns a new File whose stream is set to the standard output stream.
    */

    IoFile *newFile = IoFile_new(IOSTATE);
    DATA(newFile)->path = IOREF(IOSYMBOL("<standard output>"));
    DATA(newFile)->mode = IOREF(IOSYMBOL("w"));
    DATA(newFile)->stream = stdout;
    DATA(newFile)->flags = IOFILE_FLAGS_NONE;
    return newFile;
}

IO_METHOD(IoFile, standardError) {
    /*doc File standardError
    Returns a new File whose stream is set to the standard error stream.
    */

    IoFile *newFile = IoFile_new(IOSTATE);
    DATA(newFile)->path = IOREF(IOSYMBOL("<standard error>"));
    DATA(newFile)->mode = IOREF(IOSYMBOL("w"));
    DATA(newFile)->stream = stderr;
    DATA(newFile)->flags = IOFILE_FLAGS_NONE;
    return newFile;
}

IO_METHOD(IoFile, setPath) {
    /*doc File setPath(aString)
    Sets the file path of the receiver to pathString.
    The default path is an empty string. Returns self.
    */

    DATA(self)->path = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
    return self;
}

IO_METHOD(IoFile, path) {
    /*doc File path
    Returns the file path of the receiver.
    */

    return DATA(self)->path;
}

IO_METHOD(IoFile, lastPathComponent) {
    /*doc File name
    Returns the last path component of the file path.
    */

    return IoSeq_lastPathComponent(DATA(self)->path, locals, m);
}

IO_METHOD(IoFile, mode) {
    /*doc File mode
    Returns the open mode of the file(either read, update or append).
    */

    char *mode = IoSeq_asCString(DATA(self)->mode);

    if (!strcmp(mode, "r")) {
        return IOSYMBOL("read");
    }
    if (!strcmp(mode, "r+")) {
        return IOSYMBOL("update");
    }
    if (!strcmp(mode, "a+")) {
        return IOSYMBOL("append");
    }

    return IONIL(self);
}

IO_METHOD(IoFile, temporaryFile) {
    /*doc File temporaryFile
    Returns a new File object with an open temporary file. The file is
    automatically deleted when the returned File object is closed or garbage
    collected.
    */

    IoFile *newFile = IoFile_new(IOSTATE);
    DATA(newFile)->stream = tmpfile();
    return newFile;
}

IO_METHOD(IoFile, openForReading) {
    /*doc File openForReading(optionalPathString)
    Sets the file mode to read (reading only) and calls
    open(optionalPathString).
    */

    DATA(self)->mode = IOREF(IOSYMBOL("r"));
    return IoFile_open(self, locals, m);
}

IO_METHOD(IoFile, openForUpdating) {
    /*doc File openForUpdating(optionalPathString)
    Sets the file mode to update (reading and writing) and calls
    open(optionalPathString). This will not delete the file if it already
    exists. Use the remove method first if you need to delete an existing file
    before opening a new one.
    */

    DATA(self)->mode = IOREF(IOSYMBOL("r+"));
    return IoFile_open(self, locals, m);
}

IO_METHOD(IoFile, openForAppending) {
    /*doc File openForAppending(optionalPathString)
    Sets the file mode to append (writing to the end of the file)
    and calls open(optionalPathString).
    */

    DATA(self)->mode = IOREF(IOSYMBOL("a+"));
    return IoFile_open(self, locals, m);
}

IO_METHOD(IoFile, open) {
    /*doc File open(optionalPathString)
    Opens the file. Creates one if it does not exist.
    If the optionalPathString argument is provided, the path is set to it before
    opening. Returns self or raises an File exception on error.
    */

    char *mode = CSTRING(DATA(self)->mode);

    DATA(self)->flags = IOFILE_FLAGS_NONE;

    if (IoMessage_argCount(m) > 0) {
        DATA(self)->path = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
    }

    if (!DATA(self)->stream) {
        if (!IoFile_justExists(self) && strcmp(mode, "r") != 0) {
            IoFile_create(self);

            if (!IoFile_justExists(self)) {
                IoState_error_(IOSTATE, m, "unable to create file '%s': %s",
                               UTF8CSTRING(DATA(self)->path), strerror(errno));
            }
        }

        DATA(self)->stream = fopen(UTF8CSTRING(DATA(self)->path), mode);
    }

    if (DATA(self)->stream == NULL) {
        IoState_error_(IOSTATE, m, "unable to open file path '%s': %s",
                       UTF8CSTRING(DATA(self)->path), strerror(errno));
    }

    return self;
}

IO_METHOD(IoFile, reopen) {
    /*doc File reopen(otherFile, mode)
    Reopens otherFile and redirects its stream to this file's path using mode.
    If mode is omitted, it is copied from otherFile.
    Returns self or raises a File exception on error.
    */

    IoFile *otherFile;
    IoSeq *mode;

    DATA(self)->flags = IOFILE_FLAGS_NONE;

    IoMessage_assertArgCount_receiver_(m, 1, self);

    otherFile = IoMessage_locals_valueArgAt_(m, locals, 0);
    IOASSERT(ISFILE(otherFile), "arg must be a File");

    mode = IoMessage_locals_valueArgAt_(m, locals, 1);
    if (ISSEQ(mode)) {
        DATA(self)->mode = IOREF(mode);
    } else {
        DATA(self)->mode = IOREF(IoSeq_newWithUArray_copy_(
            IOSTATE, (UArray *)DATA(DATA(otherFile)->mode), 1));
    }

    if (!DATA(self)->stream) {
        FILE *fp = freopen(UTF8CSTRING(DATA(self)->path),
                           CSTRING(DATA(self)->mode), DATA(otherFile)->stream);

        if (fp) {
            DATA(self)->stream = fp;
        } else {
            printf("%i:%s\n", errno, strerror(errno));
            IoState_error_(
                IOSTATE, m, "unable to reopen to file '%s' with mode %s.",
                UTF8CSTRING(DATA(self)->path), CSTRING(DATA(self)->mode));
            fclose(fp);
        }
    }

    return self;
}

IO_METHOD(IoFile, popen) {
    /*doc File popen
    Open the file as a pipe. Return self.

    Closing a popen'ed file sets exitStatus or termSignal
    to reflect the status or cause of the child processes' termination.
    */

    DATA(self)->flags = IOFILE_FLAGS_PIPE;

    if (IoMessage_argCount(m) > 0) {
        DATA(self)->path = IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0));
    }

    if (DATA(self)->stream) {
        IoFile_justClose(self);
    }

#if defined(SANE_POPEN)
    DATA(self)->mode = IOREF(IOSYMBOL("a+"));
    DATA(self)->stream = popen(UTF8CSTRING(DATA(self)->path), "r+");
#elif defined(__SYMBIAN32__)
    /* Symbian does not implement popen.
     * (There is popen3() but it is "internal and not intended for use.")
     */
    DATA(self)->mode = IOREF(IOSYMBOL("r"));
    DATA(self)->stream = NULL;
#else
    DATA(self)->mode = IOREF(IOSYMBOL("r"));
    DATA(self)->stream = popen(UTF8CSTRING(DATA(self)->path), "r");
#endif
    if (DATA(self)->stream == NULL) {
        IoState_error_(IOSTATE, m, "error executing file path '%s'",
                       UTF8CSTRING(DATA(self)->path));
    }

    return self;
}

IO_METHOD(IoFile, close) {
    /*doc File close
    Closes the receiver if open, otherwise does nothing. Returns self.

    When the file was opened via popen, sets either exitStatus or
    termSignal to the exit status on normal exit, or the signal causing
    abnormal termination.
    */

    IoFile_justClose(self);
    return self;
}

IO_METHOD(IoFile, flush) {
    /*doc File flush
    Forces any buffered data to be written to disk. Returns self.
    */

    fflush(DATA(self)->stream);
    return self;
}

IoObject *IoFile_rawAsString(IoFile *self) {
    UArray *ba = UArray_new();

    if (UArray_readFromFilePath_(ba, IoSeq_rawUArray(DATA(self)->path)) == 1) {
        return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
    } else {
        UArray_free(ba);
        IoState_error_(IOSTATE, NULL, "unable to read file '%s'",
                       UTF8CSTRING(DATA(self)->path));
    }

    return IONIL(self);
}

IO_METHOD(IoFile, contents) {
    /*doc File contents
    Returns contents of the file as a mutable Sequence of bytes.
    */

    UArray *ba = UArray_new();
    long result = -1;

    if (DATA(self)->stream == stdin) {
        result = UArray_readFromCStream_(ba, DATA(self)->stream);
    } else {
        result =
            UArray_readFromFilePath_(ba, IoSeq_rawUArray(DATA(self)->path));
    }

    if (result != -1) {
        return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
    } else {
        UArray_free(ba);
        IoState_error_(IOSTATE, m, "unable to read file '%s'",
                       UTF8CSTRING(DATA(self)->path));
    }

    return IONIL(self);
}

IO_METHOD(IoFile, asBuffer) {
    /*doc File asBuffer
    Opens the receiver in read only mode, reads the whole
    contents of the file into a buffer object, closes the file and returns the
    buffer.
    */

    UArray *ba = UArray_new();
    long result =
        UArray_readFromFilePath_(ba, IoSeq_rawUArray(DATA(self)->path));

    if (-1 != result) {
        return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
    } else {
        UArray_free(ba);
        IoState_error_(IOSTATE, m, "unable to read file '%s'",
                       UTF8CSTRING(DATA(self)->path));
    }

    return IONIL(self);
}

IO_METHOD(IoFile, exists) {
    /*doc File exists(optionalPath)
    Returns true if the file path exists, and false otherwise.
    If optionalPath string is provided, it tests the existance of that path
    instead.
    */

    IoSymbol *path;

    if (IoMessage_argCount(m) > 0) {
        path = IoMessage_locals_symbolArgAt_(m, locals, 0);
    } else {
        path = DATA(self)->path;
    }

    return IOBOOL(self, fileExists(UTF8CSTRING(path)));
}

IO_METHOD(IoFile, remove) {
    /*doc File remove
    Removes the file specified by the receiver's path.
    Raises an error if the file exists but is not removed. Returns self.
    */

    int error = 0;

#if defined(__SYMBIAN32__)
    error = -1;
#elif defined(_MSC_VER) || defined(__MINGW32__)
    if (IoFile_justExists(self)) {
        if (ISTRUE(IoFile_isDirectory(self, locals, m))) {
            error = rmdir(UTF8CSTRING(DATA(self)->path));
        } else {
            error = unlink(UTF8CSTRING(DATA(self)->path));
        }
    }
#else
    error = remove(UTF8CSTRING(DATA(self)->path));
#endif

    if (error && IoFile_justExists(self)) {
        IoState_error_(IOSTATE, m, "error removing file '%s'",
                       UTF8CSTRING(DATA(self)->path));
    }
    return self;
}

IO_METHOD(IoFile, truncateToSize) {
    /*doc File truncateToSize(numberOfBytes)
    Truncates the file's size to the numberOfBytes. Returns self.
    */

    long newSize = IoMessage_locals_longArgAt_(m, locals, 0);
    truncate(UTF8CSTRING(DATA(self)->path), newSize);
    return self;
}

IO_METHOD(IoFile, moveTo_) {
    /*doc File moveTo(pathString)
    Moves the file specified by the receiver's path to the
    new path pathString. Raises a File doesNotExist exception if the
    file does not exist or a File nameConflict exception if the file
    nameString already exists.
    */

    IoSymbol *newPath = IoMessage_locals_symbolArgAt_(m, locals, 0);
    const char *fromPath = UTF8CSTRING(DATA(self)->path);
    const char *toPath = UTF8CSTRING(newPath);

    if (strcmp(fromPath, toPath) != 0) {
        int error;

        remove(toPath); // to make sure we do not get an error
        error = rename(fromPath, toPath);

        if (error) {
            IoState_error_(IOSTATE, m, "error moving file '%s' to '%s'",
                           fromPath, toPath);
        }
    }

    return self;
}

IO_METHOD(IoFile, write) {
    /*doc File write(aSequence1, aSequence2, ...)
    Writes the arguments to the receiver file. Returns self.
    */

    int i;

    IoFile_assertOpen(self, locals, m);
    IoFile_assertWrite(self, locals, m);

    for (i = 0; i < IoMessage_argCount(m); i++) {
        IoSymbol *string = IoMessage_locals_seqArgAt_(m, locals, i);
        UArray_writeToCStream_(IoSeq_rawUArray(string), DATA(self)->stream);

        if (ferror(DATA(self)->stream) != 0) {
            IoState_error_(IOSTATE, m, "error writing to file '%s'",
                           UTF8CSTRING(DATA(self)->path));
        }
    }

    return self;
}

IO_METHOD(IoFile, readLines) {
    /*doc File readLines
    Returns list containing all lines in the file.
    */

    IoState *state = IOSTATE;

    if (!DATA(self)->stream) {
        IoFile_openForReading(self, locals, m);
    }

    IoFile_assertOpen(self, locals, m);

    {
        IoList *lines = IoList_new(state);
        IoObject *newLine;

        IoState_pushRetainPool(state);

        for (;;) {
            IoState_clearTopPool(state);
            newLine = IoFile_readLine(self, locals, m);

            if (ISNIL(newLine)) {
                break;
            }

            IoList_rawAppend_(lines, newLine);
        }
        IoState_popRetainPool(state);

        return lines;
    }
}

IO_METHOD(IoFile, readLine) {
    /*doc File readLine
    Reads the next line of the file and returns it as a
    string without the return character. Returns Nil if the
    end of the file has been reached.
    */

    // char *path = UTF8CSTRING(DATA(self)->path); // tmp for debugging

    IoFile_assertOpen(self, locals, m);

    if (feof(DATA(self)->stream) != 0) {
        clearerr(DATA(self)->stream);
        return IONIL(self);
    } else {
        UArray *ba = UArray_new();
        int error;
        unsigned char didRead =
            UArray_readLineFromCStream_(ba, DATA(self)->stream);

        if (!didRead) {
            UArray_free(ba);
            return IONIL(self);
        }

        error = ferror(DATA(self)->stream);

        if (error != 0) {
            UArray_free(ba);
            clearerr(DATA(self)->stream);
            IoState_error_(IOSTATE, m, "error reading from file '%s'",
                           UTF8CSTRING(DATA(self)->path));
            return IONIL(self);
        }

        return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
        /*return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);*/
    }
}

UArray *IoFile_readUArrayOfLength_(IoFile *self, IoObject *locals,
                                   IoMessage *m) {
    size_t length = IoMessage_locals_sizetArgAt_(m, locals, 0);
    UArray *ba = UArray_new();
    IoFile_assertOpen(self, locals, m);

    UArray_readNumberOfItems_fromCStream_(ba, length, DATA(self)->stream);

    if (ferror(DATA(self)->stream) != 0) {
        clearerr(DATA(self)->stream);
        UArray_free(ba);
        IoState_error_(IOSTATE, m, "error reading file '%s'",
                       UTF8CSTRING(DATA(self)->path));
    }

    if (!UArray_size(ba)) {
        UArray_free(ba);
        return NULL;
    }

    return ba;
}

IO_METHOD(IoFile, readToBufferLength) {
    /*doc File readToBufferLength(aBuffer, aNumber)
    Reads at most aNumber number of items and appends them to aBuffer.
    Returns number of items read.
    */

    IoSeq *buffer = IoMessage_locals_mutableSeqArgAt_(m, locals, 0);
    size_t length = IoMessage_locals_longArgAt_(m, locals, 1);
    UArray *ba = IoSeq_rawUArray(buffer);
    size_t itemsRead =
        UArray_readNumberOfItems_fromCStream_(ba, length, DATA(self)->stream);
    return IONUMBER(itemsRead);
}

IO_METHOD(IoFile, readBufferOfLength_) {
    /*doc File readBufferOfLength(aNumber)
    Reads a Buffer of the specified length and returns it.
    Returns Nil if the end of the file has been reached.
    */

    UArray *ba = IoFile_readUArrayOfLength_(self, locals, m);

    if (!ba) {
        return IONIL(self);
    }

    return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoFile, readStringOfLength_) {
    /*doc File readStringOfLength(aNumber)
    Reads a String of the specified length and returns it.
    Returns Nil if the end of the file has been reached.
    */

    UArray *ba = IoFile_readUArrayOfLength_(self, locals, m);

    if (!ba) {
        return IONIL(self);
    }

    return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoFile, rewind) {
    /*doc File rewind
    Sets the file position pointer to the beginning of the file.
    */

    IoFile_assertOpen(self, locals, m);

    if (DATA(self)->stream) {
        rewind(DATA(self)->stream);
    }

    return self;
}

IO_METHOD(IoFile, position_) {
    /*doc File setPosition(aNumber)
    Sets the file position pointer to the byte specified by aNumber. Returns
    self.
    */

    long pos = IoMessage_locals_longArgAt_(m, locals, 0);
    IoFile_assertOpen(self, locals, m);

    if (fseek(DATA(self)->stream, pos, 0) != 0) {
        IoState_error_(IOSTATE, m, "unable to set position %i file path '%s'",
                       (int)pos, UTF8CSTRING(DATA(self)->path));
    }

    return self;
}

IO_METHOD(IoFile, position) {
    /*doc File position
    Returns the current file pointer byte position as a Number.
    */

    IoFile_assertOpen(self, locals, m);
    return IONUMBER(ftell(DATA(self)->stream));
}

IO_METHOD(IoFile, positionAtEnd) {
    /*doc File positionAtEnd
    Sets the file position pointer to the end of the file.
    */

    IoFile_assertOpen(self, locals, m);

    if (DATA(self)->stream) {
        fseek(DATA(self)->stream, 0, SEEK_END);
    }

    return self;
}

IO_METHOD(IoFile, isAtEnd) {
    /*doc File isAtEnd
    Returns true if the file is at its end. Otherwise returns false.
    */

    IoFile_assertOpen(self, locals, m);
    return IOBOOL(self, feof(DATA(self)->stream) != 0);
}

IO_METHOD(IoFile, size) {
    /*doc File size
    Returns the file size in bytes.
    */

    FILE *fp = fopen(UTF8CSTRING(DATA(self)->path), "r");

    if (fp) {
        long fileSize;
        fseek(fp, 0, SEEK_END);
        fileSize = ftell(fp);
        fclose(fp);
        return IONUMBER(fileSize);
    } else {
        IoState_error_(IOSTATE, m, "unable to open file '%s'",
                       UTF8CSTRING(DATA(self)->path));
    }

    return IONIL(self);
}

IO_METHOD(IoFile, isOpen) {
    /*doc File isOpen
    Returns self if the file is open. Otherwise returns Nil.
    */

    return IOBOOL(self, DATA(self)->stream != 0);
}

IO_METHOD(IoFile, assertOpen) {
    if (!DATA(self)->stream) {
        IoState_error_(IOSTATE, m, "file '%s' not yet open",
                       UTF8CSTRING(DATA(self)->path));
    }
    return self;
}

IO_METHOD(IoFile, assertWrite) {
    char *mode = IoSeq_asCString(DATA(self)->mode);

    if ((strcmp(mode, "r+")) && (strcmp(mode, "a+")) && (strcmp(mode, "w"))) {
        IoState_error_(IOSTATE, m, "file '%s' not open for write",
                       UTF8CSTRING(DATA(self)->path));
    }

    return self;
}

IO_METHOD(IoFile, at) {
    /*doc File at(aNumber)
    Returns a Number containing the byte at the specified
    byte index or Nil if the index is out of bounds.
    */

    int byte;

    IoFile_assertOpen(self, locals, m);
    IoFile_position_(self, locals, m); /* works since first arg is the same */
    byte = fgetc(DATA(self)->stream);

    if (byte == EOF) {
        return IONIL(self);
    }

    return IONUMBER(byte);
}

IO_METHOD(IoFile, atPut) {
    /*doc File atPut(positionNumber, byteNumber)
    Writes the byte value of byteNumber to the file position
    positionNumber. Returns self.
    */

    int c = IoMessage_locals_intArgAt_(m, locals, 1);

    IoFile_assertOpen(self, locals, m);
    IoFile_assertWrite(self, locals, m);
    IoFile_position_(self, locals, m); // works since first arg is the same

    if (fputc(c, DATA(self)->stream) == EOF) {
        int pos = IoMessage_locals_intArgAt_(
            m, locals, 0); // BUG - this may not be the same when evaled
        IoState_error_(IOSTATE, m, "error writing to position %i in file '%s'",
                       pos, UTF8CSTRING(DATA(self)->path));
    }

    return self;
}

IO_METHOD(IoFile, foreach) {
    /*doc File foreach(optionalIndex, value, message)
    For each byte, set index to the index of the byte
and value to the number containing the byte value and execute aMessage.
Example usage:
<p>
<pre>
aFile foreach(i, v, writeln("byte at ", i, " is ", v))
aFile foreach(v, writeln("byte ", v))
</pre>
*/
    IoObject *result;

    IoSymbol *indexSlotName, *characterSlotName;
    IoMessage *doMessage;
    int i = 0;

    IoFile_assertOpen(self, locals, m);

    result = IONIL(self);

    IoMessage_foreachArgs(m, self, &indexSlotName, &characterSlotName,
                          &doMessage);

    for (;;) {
        int c = getc(DATA(self)->stream);

        if (c == EOF) {
            break;
        }

        if (indexSlotName) {
            IoObject_setSlot_to_(locals, indexSlotName, IONUMBER(i));
        }

        IoObject_setSlot_to_(locals, characterSlotName, IONUMBER(c));
        result = IoMessage_locals_performOn_(doMessage, locals, locals);

        if (IoState_handleStatus(IOSTATE)) {
            break;
        }

        i++;
    }
    return result;
}

IO_METHOD(IoFile, foreachLine) {
    /*doc File foreachLine(optionalLineNumber, line, message)
    For each line, set index to the line number of the line
and line and execute aMessage.
Example usage:
<pre>
aFile foreachLine(i, v, writeln("Line ", i, ": ", v))
aFile foreach(v, writeln("Line: ", v))
</pre>
*/

    IoObject *result;

    IoSymbol *indexSlotName, *lineSlotName;
    IoMessage *doMessage;
    IoObject *newLine;
    int i = 0;

    IoState *state;

    IoFile_assertOpen(self, locals, m);

    IoMessage_foreachArgs(m, self, &indexSlotName, &lineSlotName, &doMessage);

    result = IONIL(self);
    state = IOSTATE;

    IoState_pushRetainPool(state);

    for (;;) {
        IoState_clearTopPool(state);
        newLine = IoFile_readLine(self, locals, m);

        if (ISNIL(newLine)) {
            break;
        }

        if (indexSlotName) {
            IoObject_setSlot_to_(locals, indexSlotName, IONUMBER(i));
        }
        IoObject_setSlot_to_(locals, lineSlotName, newLine);

        result = IoMessage_locals_performOn_(doMessage, locals, locals);
        if (IoState_handleStatus(IOSTATE)) {
            break;
        }
        i++;
    }

    IoState_popRetainPool(state);
    return result;
}

/*
IO_METHOD(IoFile, makeUnbuffered)
{
        // doc File makeUnbuffered Sets the file's stream to be unbuffered.
Returns self.

        setvbuf(DATA(self)->stream, NULL, _IONBF, 0);
        // this doesn't work on stdin and there is no OS neutral way to get
unbuffered input

        return self;
}

IO_METHOD(IoFile, makeLineBuffered)
{
        // doc File makeLineBuffered Sets the file's stream to be line buffered.
Returns self.

        setvbuf(DATA(self)->stream, NULL, _IOLBF, 0);

        return self;
}

IO_METHOD(IoFile, makeFullyBuffered)
{
        // doc File makeFullyBuffered Sets the file's stream to be fully
buffered. Returns self.

        setvbuf(DATA(self)->stream, NULL, _IOFBF, 0);

        return self;
}
*/
