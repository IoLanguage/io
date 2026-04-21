# Exception

The Exception proto is used for raising exceptions; instances hold exception-related info.

<p><b>Raise</b><p>

An exception can be raised by calling raise() on an exception proto.
Exception raise("generic foo exception")

<p><b>Try and Catch</b><p>

To catch an exception, the try() method of the Object proto is used. try() will catch any exceptions that occur within it and return the caught exception or nil if no exception is caught.

<pre>
e := try(<doMessage>)
</pre>

To catch a particular exception, the Exception catch() method can be used. Example:

<pre>
e := try(
// ...
)

e catch(Exception,
writeln(e coroutine backtraceString)
)
</pre>

The first argument to catch indicates which types of exceptions will be caught. catch() returns the exception if it doesn't match and nil if it does.

<p><b>Pass</b><p>

To re-raise an exception caught by try(), use the pass method. This is useful to pass the exception up to the next outer exception handler, usually after all catches failed to match the type of the current exception:
<pre>
e := try(
// ...
)

e catch(Error,
// ...
) catch(Exception,
// ...
) pass
</pre>

<p><b>Custom Exceptions</b><p>

Custom exception types can be implemented by simply cloning an existing Exception type:
<pre>
MyErrorType := Error clone
</pre>

## _findHandler

Walks handler stacks from the current coroutine up through the parentCoroutine chain.
Returns the first matching handler block for this exception type, or nil.

## _Resumption

A simple object whose invoke(v) returns v.

## catch(exceptionProto)

Catch an exception with the specified exception prototype.

## caughtMessage

Returns the message object associated with the exception.

## error

Returns the coroutine that the exception occurred in.

## nestedException

Returns the nestedException if there is one.

## originalCall

Returns the call object associated with the exception.

## pass

Pass the exception up the stack.

## raise(error,

optionalNestedException) Raise an exception with the specified error message.

## showStack

Print the exception and related stack.

## signal(error, optionalNestedException)

Raises a resumable exception. If a matching handler is installed via withHandler,
the handler is called with (exception, resume). The handler can resume
execution at the signal site by calling resume invoke(value), or simply returning
a value (which auto-resumes). If no handler is found, falls back to non-resumable raise.

