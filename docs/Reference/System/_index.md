# System

Contains methods related to the IoVM.

- **category**: Core
- **copyright**: Steve Dekorte, 2002
- **license**: BSD revised

## activeCpus

Returns the number of active CPUs.

## args

Returns the list of command line argument strings the program was run
with.

## distribution

Returns the Io distribution name as a string.

## errorNumber

Returns the C errno string.

## exit(optionalReturnCodeNumber)

Shutdown the IoState (io_free all objects) and return
control to the calling program (if any).

## getEnvironmentVariable(nameString)

Returns a string with the value of the environment
variable whose name is specified by nameString.

## getOptions(args)

This primitive is used to get command line options similar to Cs getopt().
It returns a map in containing the left side of the argument, with the
value of the right side. (The key will not contain
the beginning dashes (--).
<p>
Example:
<pre>
options := System getOptions(args)
options foreach(k, v,
if(v type == List type,
v foreach(i, j, writeln(\"Got unnamed argument with value: \" .. j))
continue
)
writeln(\"Got option: \" .. k .. \" with value: \" .. v)
)
</pre>

## installPrefix

Returns the root path where io was installed. The default is /usr/local.

## ioPath

Returns the path of io installation. The default is $INSTALL_PREFIX/lib/io.

## iospecVersion

The version of IoSpec our IoVM is compatible with

## iovmName

The name of our IoVM as used by IoSpec

## launchPath

Returns a pathComponent of the launch file.

## launchScript

Returns the path of the io file run on the command line. Returns nil if no file was run.

## maxRecycledObjects

Returns the max number of recycled objects used.

## platform

Returns a string description of the platform.

## platformVersion

Returns the version id of the OS.

## recycledObjectCount

Returns the current number of objects being held for recycling.

## runCommand(cmd,

successStatus) Calls system and redirects stdout/err to tmp files.  Returns object with exitStatus, stdout and stderr slots.

## setEnvironmentVariable(keyString, valueString)

Sets the environment variable keyString to the value valueString.

## setLobby(anObject)

Sets the root object of the garbage collector.

## setMaxRecycledObjects(aNumber)

Sets the max number of recycled objects used.

## sleep(secondsNumber)

Performs a *blocking* sleep call for specified number of seconds.

## symbols

Returns a List containing all Symbols currently in the system.

## system(aString)

Makes a system call and returns a Number for the return value.

## thisProcessPid()

Return the process id (pid) for this Io process.

## userInterruptHandler

Called when control-c is hit. Override to add custom behavior. Returns self.

## version

Returns a version number for Io.

