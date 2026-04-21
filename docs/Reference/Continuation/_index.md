# Continuation

A first-class continuation captured by `callcc`. Represents a resumable
snapshot of execution state; invoking it re-enters the captured context.

## asMap

Returns a Map representation of the captured continuation state.

## copy

Returns a deep copy of this continuation. The copy has an independent
frame chain that can be invoked separately. Use this for multi-shot
or delayed invocation patterns.

Example:
saved := nil
callcc(block(cont,
saved = cont copy  // deep copy while frames are live
"normal"
))
// Later: saved invoke("delayed")

## frameCount

Returns the number of frames in the captured continuation stack.
Returns 0 if no state has been captured.

## frameMessages

Returns a list of strings showing the current message at each
frame in the captured continuation stack (from top to bottom).

## frameStates

Returns a list of strings describing the state of each frame
in the captured continuation stack (from top to bottom).

## fromMap(aMap)

Restores a Continuation from a Map representation produced by asMap.
Returns self with the restored continuation state.

## invoke(value)

Invokes the continuation, restoring the captured execution state.
The value argument becomes the result of the original callcc call.
Continuations are one-shot: they can only be invoked once.
Use copy to create a fresh continuation for multiple invocations.

## isInvoked

Returns true if this continuation has been invoked.

