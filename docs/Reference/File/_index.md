# File

Encapsulates file i/o. Here's an example of opening a file,
and reversing its lines:
<pre>
file := File clone openForUpdating("/tmp/test")
lines := file readLines reverse
file rewind
lines foreach(line, file write(line, "\n"))
file close
</pre>

## appendToContents(aSeq)

Appends to the contents of the file with aSeq. Returns self.

## asBuffer

Opens the receiver in read only mode, reads the whole
contents of the file into a buffer object, closes the file and returns the
buffer.

## at(aNumber)

Returns a Number containing the byte at the specified
byte index or Nil if the index is out of bounds.

## atPut(positionNumber, byteNumber)

Writes the byte value of byteNumber to the file position
positionNumber. Returns self.

## baseName

Returns File's name without an extension (returned Sequence consists of all characters up to final period ["."] character).

## close

Closes the receiver if open, otherwise does nothing. Returns self.

When the file was opened via popen, sets either exitStatus or
termSignal to the exit status on normal exit, or the signal causing
abnormal termination.

## containingDirectory

Deprecated. See parentDirectory.

## contents

Returns contents of the file as a mutable Sequence of bytes.

## copyToPath(destinationPath)

Copies the file to the specified path.

## copyToPathWithoutYielding(destinationPath)

Copies the file to the specified path without yielding.

## create

Creates an empty file at the file's path. Returns self on success, nil on failure.

## descriptor

Returns the file's descriptor as a number.

## exists(optionalPath)

Returns true if the file path exists, and false otherwise.
If optionalPath string is provided, it tests the existance of that path
instead.

## exitStatus

System's close status (after file close).

## flush

Forces any buffered data to be written to disk. Returns self.

## foreach(optionalIndex, value, message)

For each byte, set index to the index of the byte
and value to the number containing the byte value and execute aMessage.
Example usage:
<p>
<pre>
aFile foreach(i, v, writeln("byte at ", i, " is ", v))
aFile foreach(v, writeln("byte ", v))
</pre>

## foreachLine(optionalLineNumber, line, message)

For each line, set index to the line number of the line
and line and execute aMessage.
Example usage:
<pre>
aFile foreachLine(i, v, writeln("Line ", i, ": ", v))
aFile foreach(v, writeln("Line: ", v))
</pre>

## groupId

Returns a Number containing the group id associated with the file's path.

## isAtEnd

Returns true if the file is at its end. Otherwise returns false.

## isDirectory

Returns true if the receiver's path points to a directory, false otherwise.

## isLink

Returns true if the receiver's path points to a link, false otherwise.

## isOpen

Returns self if the file is open. Otherwise returns Nil.

## isPipe

Returns true if the receiver is a pipe, false otherwise.

## isRegularFile

Returns true if the receiver's file descriptor is a regular file, false
otherwise.

## isSocket

Returns true if the receiver's file descriptor is a Socket, false otherwise.

## isUserExecutable

Returns true if the receiver is user group executable, false otherwise.

## lastAccessDate

Returns a Date object containing the last date and
time the file was accessed.

## lastDataChangeDate

Returns a Date object containing the last date and
time the file's contents were changed.

## lastInfoChangeDate

Returns a Date object containing the last date and
time the file's meta info was changed.

## lock

Creates a path.lock file if it does not exist. If it does exist, it waits for it be removed by the process that created it first. Returns self.

## lockFile

Deprecated. The .lock file used for File locking.

## makeFullyBuffered

Sets the file's stream to be fully

## makeLineBuffered

Sets the file's stream to be line buffered.

## makeUnbuffered

Sets the file's stream to be unbuffered.

## mode

Returns the open mode of the file(either read, update or append).

## moveTo(pathString)

Moves the file specified by the receiver's path to the
new path pathString. If a file already exists at pathString,
it is silently overwritten. Raises a File error exception if
the rename fails.

## name

Returns the last path component of the file path.

## open(optionalPathString)

Opens the file. Creates one if it does not exist.
If the optionalPathString argument is provided, the path is set to it before
opening. Returns self or raises an File exception on error.

## openForAppending(optionalPathString)

Sets the file mode to append (writing to the end of the file)
and calls open(optionalPathString).

## openForReading(optionalPathString)

Sets the file mode to read (reading only) and calls
open(optionalPathString).

## openForUpdating(optionalPathString)

Sets the file mode to update (reading and writing) and calls
open(optionalPathString). This will not delete the file if it already
exists. Use the remove method first if you need to delete an existing file
before opening a new one.

## parentDirectory

Returns a File for the directory that is the parent directory of this object's path.

## path

Returns the file path of the receiver.

## popen

Open the file as a pipe. Return self.

Closing a popen'ed file sets exitStatus or termSignal
to reflect the status or cause of the child processes' termination.

## position

Returns the current file pointer byte position as a Number.

## positionAtEnd

Sets the file position pointer to the end of the file.

## protectionMode

Returns a Number containing the protection mode
associated with the file's path.

## readBufferOfLength(aNumber)

Reads a Buffer of the specified length and returns it.
Returns Nil if the end of the file has been reached.

## readLine

Reads the next line of the file and returns it as a
string without the return character. Returns Nil if the
end of the file has been reached.

## readLines

Returns list containing all lines in the file.

## readStringOfLength(aNumber)

Reads a String of the specified length and returns it.
Returns Nil if the end of the file has been reached.

## readToBufferLength(aBuffer, aNumber)

Reads at most aNumber number of items and appends them to aBuffer.
Returns number of items read.

## readToEnd(chunkSize)

Reads chunkSize bytes (4096 by default) at a time until end of file is reached.  Returns a sequence containing the bytes read from the file.

## remove

Removes the file specified by the receiver's path.
Raises an error if the file exists but is not removed. Returns self.

## reopen(otherFile, mode)

Reopens otherFile and redirects its stream to this file's path using mode.
If mode is omitted, it is copied from otherFile.
Returns self or raises a File exception on error.

## rewind

Sets the file position pointer to the beginning of the file.

## setContents(aSeq)

Replaces the contents for the file with aSeq. Returns self.

## setPath(aString)

Sets the file path of the receiver to pathString.
The default path is an empty string. Returns self.

## setPosition(aNumber)

Sets the file position pointer to the byte specified by aNumber. Returns
self.

## setStreamDestination(aBuffer)

Set the buffer to be used for future stream operations.

## size

Returns the file size in bytes.

## standardError

Returns a new File whose stream is set to the standard error stream.

## standardInput

Returns a new File whose stream is set to the standard input stream.

## standardOutput

Returns a new File whose stream is set to the standard output stream.

## startStreaming

Begin streamed read to stream destination set by setStreamDestination(aBuffer).

## stat

Updates the receiver's meta info cache.

## statSize

Returns the file's size in bytes as a Number.

## streamDestination

The buffer object to be used for future stream operations.

## streamReadSize

Size of stream buffer used for streaming operations

## streamTo(aBuffer)

Perform streamed reading to given buffer. The buffer will be appended with chunks of size streamReadSize until EOF. The final chunk may be less than streamReadSize.

## streamToWithoutYielding(aBuffer)

Perform streamed reading to given buffer without yielding between reads.

## temporaryFile

Returns a new File object with an open temporary file. The file is
automatically deleted when the returned File object is closed or garbage
collected.

## termSignal

Pipe's terminating signal (after file close opened via popen).

## thisSourceFile

Returns a File representing the system file in which this Io code exists.

## truncateToSize(numberOfBytes)

Truncates the file's size to the numberOfBytes. Returns self.

## userId

Returns a Number containing the user id associated with the file's path.

## with(aPath)

Returns a new instance with the provided path.

## write(aSequence1, aSequence2, ...)

Writes the arguments to the receiver file. Returns self.

