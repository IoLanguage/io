# Directory

The Directory object supports accessing filesystem directories. A note on paths;
if a path begins with a "/" it's the root,
if it beings with a "./" it's the launch path,
if not specified, "./" is assumed.""")

## accessibleAncestors

Returns a list of accessible ancestor directories.

## accessibleParents

Deprecated. Replaced by accessibleAncestors.

## ancestorDirectories

Returns a list of ancestor directories.

## at(aString)

Returns a File or Directory object matching the name specified
by aString or Nil if no such file or directory exists.

## create

Create the directory if it doesn't exist.
Returns self on success (or if the directory already exists), nil on
failure.

## createFileNamed(name)

Deprecated. Returns a new file object in the directory with the specified name.

## createIfAbsent

Creates the directory (and any missing ancestor directories) if they don't exist. Returns self.

## createSubdirectory(name)

Create a subdirectory with the specified name.

## currentWorkingDirectory

Returns the current working directory path.

## directories

Returns a list of subdirectories in the receiver.

## directoryNamed(aName)

Returns a Directory object for the specified file name whether or not it exists.

## exists(optionalPath)

Returns true if the Directory path exists, and false otherwise.
If optionalPath string is provided, it tests the existence of that path
instead.

## fileNamed(aName)

Returns a File object for the specified file name whether it exists or not.

## fileNames

Returns a list of strings for the names of the files (no subdirectories) in the directory.

## files

Returns a list of File objects for the files in the Directory (no directories).

## filesWithExtension(aString)

Returns a list of File objects for the files in the Directory (no directories)
with the specified file extension.

## folderNamed(aName)

Deprecated. Replaced by directoryNamed().

## folderNamedCreateIfAbsent(name)

Deprecated. Returns a new Directory object in the directory with the specified name.

## folders

Deprecated. Replaced by directories().

## isAccessible

Returns true if the directory can be accessed, false otherwise.

## items

Returns a list object containing File and Directory objects
for the files and directories of the receiver's path.

## localItems

All subitems excluding '.' and '..'.

## moveTo(aPath)

Moves the directory to the specified path.

## name

Returns the receiver's last path component.

## parentDirectory

Returns the parent directory object or nil if there is no parent directory.

## parents

Deprecated. Replaced by ancestorDirectories.

## path

Returns the directory path. The default path is '.'.

## recursiveFilesOfTypes(suffixes)

Returns a list containing all files in the directory or any subdirectory that
has any of the specified suffixes.

## remove

Removes the directory.

## setCurrentWorkingDirectory(pathString)

Sets the current working directory path.
Returns true on success or false on error.

## setPath(aString)

Sets the directory path. Returns self.

## size

Returns a Number containing the number of file and directory
objects at the receiver's path.

## walk

Recursively walks the current directory, and executes a given callback on
each item (either File or Directory) found, excluding "." and "..".
<pre>
Io> Directory walk(println)
A0_List.io
A1_OperatorTable.io
...
Io> Directory walk(f, if(f name startsWithSeq("Directory"), f println))
Directory.io
</pre>

## with(aPath)

Returns a new instance with the provided path.

