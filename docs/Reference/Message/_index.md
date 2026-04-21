# Message

A Message object encapsulates the action of a message send.
Blocks are composed of a Message and its children.
<p>
Terminology
<pre>
Example:
Io> msg := message(A B(C D); E F)
==> A B(C D); E F

In the above example:
msg name            =>  A
msg next            =>  B(C D); E F
msg next arguments  =>  list(C D)
msg next next name  =>  ;
msg next next next  =>  E F
</pre>
Important: Modifying the message tree of a block currently in use may cause
a crash if a garbage collection cycle occurs. If the implementation were
changed to retain every called message, this could be avoided.
But the cost to performance seems to outweigh the need to cover this case for
now.

## appendArg(aMessage)

Adds aMessage to the argument list of receiver. Examples:
<pre>
Io> message(a) appendArg(message(b))
==> a(b)

Io> message(a(1,2)) appendArg(message(3))
==> a(1, 2, 3)
</pre>

## appendCachedArg(aValue)

Adds aValue to the argument list of receiver as a cachedResult.

## argAt(indexNumber)

Returns Message object for the specified argument or Nil if none exists.

## argCount

Returns the number of arguments this message has. A faster way to do, msg
arguments size. Examples, <pre> Io> message(a(1,2,3)) argCount
==> 3

Io> message(a) argCount
==> 0
</pre>

## argsEvaluatedIn(anObject)

Returns a List containing the argument messages evaluated in the
context of anObject.

## arguments

Returns a list of the message objects that act as the
receiver's arguments. Modifying this list will not alter the actual
list of arguments. Use the arguments_() method to do that.

## asMessageWithEvaluatedArgs(optionalContext)

Returns a copy of receiver with arguments evaluated in the context of
sender if optionalContext is nil.

## asSimpleString

Returns one-line string representation up to 40 characters long.

## asStackEntry

Returns a string containing message name, file and line.

## asString

Same as code().

## cachedResult

Returns the cached result of the Message or Nil if there is none.

## characterNumber

Returns the message character number. The character number is typically
the beginning character index in the source text from which the message was
read.

## clone

Returns a Message that is a deep copy of the receiver.

## code

Returns a String containing a decompiled code representation of the
receiver.

## codeOfLength(n)

Same as <tt>Message code</tt>, but returns first <tt>n</tt> characters only.

## description

Returns a string containing a short description of the method.

## doInContext(anObject, locals)

Evaluates the receiver in the context of anObject. Optional <tt>locals</tt>
object is used as message sender. <tt>anObject</tt> is used as sender
otherwise.

## evaluatedArgs

Returns a List containing the argument messages evaluated in the context.

## fromString(aString)

Returns a new Message object for the compiled(but not executed)
result of aString.

## hasCachedResult

Returns true if there is a cached result. Nil is a valid cached result.

## isEndOfLine

Returns true if the message marks the end of the line. A ';' message.

## label

Returns the message label. The label is typically set to the
name of the file from which the source code for the message was read.

## last

Returns the last message in the chain.

## lastBeforeEndOfLine

Returns the last message in the chain before the EndOfLine or nil.

## lineNumber

Returns the line number of the message. The character number
is typically the line number in the source text from which the message was
read.

## name

Returns the name of the receiver.

## next

Returns the next message in the message chain or nil if there is no next
message.

## nextIgnoreEndOfLines

Returns the next message in the message chain which is not an
EndOfLine or nil if there is no next message.

## previous

Returns the previous message in the message chain or Nil if there is no
previous message.

## removeCachedResult

Removes the cached result of the Message.

## setArguments(aListOfMessages)

Sets the arguments of the receiver to deep copies of
those contained in aListOfMessages.  Returns self.

## setCachedResult(anObject)

Sets the cached result of the message. Returns self.

## setCharacterNumber(aNumber)

Sets the character number of the message. Returns self.

## setLabel(aString)

Sets the label of the message and its children. Returns self.

## setLineNumber(aNumber)

Sets the line number of the message. Returns self.

## setName(aString)

Sets the name of the receiver. Returns self.

## setNext(aMessageOrNil)

Sets the next message in the message chain to a deep copy of
aMessage or it removes the next message if aMessage is nil.

## setPrevious(aMessageOrNil)

Sets the previous message in the message chain to a deep copy of
aMessage or it removes the previous message if aMessage is Nil.

## union(other) Creates a union of the receiver and the other parameter.

Returns a new message object with the receivers message as the first argument of the returned message,
and the other parameter's arguments as each successive argument to the new message.
<br/>
<pre>
Io> message(a) union(message(b))
==> [unnamed](a, b)
</pre>

