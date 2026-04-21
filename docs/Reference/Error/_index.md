# Error

An object that contains error information and flow control based on errors.

- **category**: Core
- **copyright**: Rich Collins 2008
- **license**: BSD revised

## ifError(code)

Evaluates code in the context of sender. Returns self.
<br/>
Another form is <tt>Error ifError(error, code)</tt>.
Note: <tt>error</tt> slot is set in the context of sender, Locals object is not created!

## isError

Returns true.

## raiseIfError

Raises exception.

## returnIfError

Returns self from the context of sender.

## with(message)

Returns new error with message slot set.

