# Call

Call stores slots related to activation.

## activated

Returns the activated value.

## argAt(argNumber)

Returns the message's argNumber arg. Shorthand for same as call message
argAt(argNumber).

## argCount

Returns the number of arguments for the call. Shortcut for "call message argCount".

## coroutine

Returns the coroutine in which the message was sent.

## delegateTo(target, altSender)

Sends the call's message to target (and relays it's stop status).
The sender is set to altSender, if it is supplied.
Returns the result of the message.

## delegateToMethod(target, methodName)

Sends the call's message to target via the method specified by methodName.
Returns the result of the message.

## description

Returns a description of the receiver as a String.

## evalArgAt(argNumber)

Evaluates the specified argument of the Call's message in the context of its
sender.

## evalArgs

Returns a list containing the call message arguments evaluated in the context of the sender.

## hasArgs

Returns true if the call was passed arguments.

## message

Returns the message value.

## relayStopStatus(arg)

Sets sender's stop status (Normal, Return,
Break, Continue etc.) and returns evaluated argument.

## resetStopStatus(arg)

Sets stop status to Normal.
See also <tt>Call setStopStatus</tt>.

## sender

Returns the sender value.

## setStopStatus(aStatusObject)

Sets the stop status on the call.

## slotContext

Returns the slotContext value.

## stopStatus

Returns the stop status on the call. (description of stopStatus will
be added once we decide whether or not to keep it)

## target

Returns the target value.

