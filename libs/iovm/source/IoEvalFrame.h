
// metadoc EvalFrame copyright Steve Dekorte 2002, 2025
// metadoc EvalFrame license BSD revised

#ifndef IOEVALFRAME_DEFINED
#define IOEVALFRAME_DEFINED 1

#include "Common.h"
#include "IoObject.h"
#include "IoMessage.h"
#include "IoCall.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
typedef struct IoEvalFrame IoEvalFrame;

// Frame states for the evaluation state machine
typedef enum {
    FRAME_STATE_START,           // Just pushed, need to begin evaluation
    FRAME_STATE_EVAL_ARGS,       // Evaluating arguments lazily
    FRAME_STATE_LOOKUP_SLOT,     // Ready to do slot lookup
    FRAME_STATE_ACTIVATE,        // Activating a block/method
    FRAME_STATE_CONTINUE_CHAIN,  // Continue to next message in chain
    FRAME_STATE_RETURN,          // Returning result to parent

    // Control flow states (for non-reentrant primitives)
    FRAME_STATE_IF_EVAL_CONDITION,    // Evaluating 'if' condition
    FRAME_STATE_IF_CONVERT_BOOLEAN,   // Converting condition result to boolean
    FRAME_STATE_IF_EVAL_BRANCH,       // Evaluating 'if' true/false branch
    FRAME_STATE_WHILE_EVAL_CONDITION, // Evaluating 'while' condition
    FRAME_STATE_WHILE_EVAL_BODY,      // Evaluating 'while' body
    FRAME_STATE_LOOP_EVAL_BODY,       // Evaluating 'loop' body
    FRAME_STATE_FOR_EVAL_SETUP,       // Evaluating 'for' setup (range/collection)
    FRAME_STATE_FOR_EVAL_BODY         // Evaluating 'for' body
} IoFrameState;

// Evaluation frame - represents one activation record
struct IoEvalFrame {
    // Message chain being evaluated
    IoMessage *message;          // Current message in chain

    // Evaluation context
    IoObject *target;            // Current target (self for this message)
    IoObject *locals;            // Current locals (sender context)
    IoObject *cachedTarget;      // Original target (for semicolon reset)

    // Parent frame (for returning)
    IoEvalFrame *parent;

    // Current state in the evaluation state machine
    IoFrameState state;

    // Argument evaluation state (for lazy evaluation)
    int argCount;                // Total number of args
    int currentArgIndex;         // Which arg we're currently evaluating
    IoObject **argValues;        // Evaluated argument values (array)

    // Result
    IoObject *result;            // Result of this frame

    // For slot lookup and activation
    IoObject *slotValue;         // Result of slot lookup
    IoObject *slotContext;       // Context where slot was found

    // Call object (for introspection - created during block activation)
    IoCall *call;                // Call object for this activation (or NULL)

    // For block activation
    IoObject *blockLocals;       // Block's local context (NULL if not a block)
    int passStops;               // Whether to pass stop status (break/continue/return)

    // Control flow continuation info (only one active at a time)
    union {
        struct {
            IoMessage *conditionMsg;      // Condition message to evaluate
            IoMessage *trueBranch;        // True branch message
            IoMessage *falseBranch;       // False branch message (may be NULL)
            int conditionResult;          // Result of condition evaluation
        } ifInfo;

        struct {
            IoMessage *conditionMsg;      // Condition message to evaluate
            IoMessage *bodyMsg;           // Body message to execute
            int conditionResult;          // Result of condition evaluation
        } whileInfo;

        struct {
            IoMessage *bodyMsg;           // Body message to execute
        } loopInfo;

        struct {
            IoMessage *setupMsg;          // Range/collection to iterate over
            IoMessage *bodyMsg;           // Body message to execute
            IoObject *counter;            // Current counter/iterator value
            IoObject *collection;         // Collection being iterated
            int currentIndex;             // Current iteration index
            int collectionSize;           // Total size of collection
        } forInfo;
    } controlFlow;
};

// Frame management functions
IOVM_API IoEvalFrame *IoEvalFrame_new(void);
IOVM_API void IoEvalFrame_free(IoEvalFrame *self);
IOVM_API void IoEvalFrame_mark(IoEvalFrame *self);
IOVM_API void IoEvalFrame_reset(IoEvalFrame *self);

#ifdef __cplusplus
}
#endif
#endif
