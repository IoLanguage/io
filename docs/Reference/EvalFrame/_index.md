# EvalFrame

Returns a human-readable description of this frame, including
the message name, state, and source location.

- **copyright**: Steve Dekorte 2002, 2025
- **license**: BSD revised
- **category**: Core

## blockLocals

Returns the block's local context object, or nil if this frame
is not a block activation.

## call

Returns the Call object for this frame (created during block/method
activation), or nil if this is not a block activation frame.

## depth

Returns the number of frames from this frame to the bottom
of the stack (inclusive). The bottom frame has depth 1.

## locals

Returns the locals object (sender context) for this frame.

## message

Returns the Message being evaluated in this frame.

## parent

Returns the parent frame, or nil if this is the bottom frame.

## result

Returns the current result value of this frame, or nil.

## state

Returns the current state of this frame as a Symbol
(e.g. "start", "activate", "if:evalBranch").

## target

Returns the target (receiver) of the message in this frame.

