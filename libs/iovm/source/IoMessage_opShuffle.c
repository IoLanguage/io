// metadoc Message copyright Jonathan Wright; Steve Dekorte 2002, 2006
// metadoc Message license BSD revised

/*cmetadoc Message description
Operator-precedence rewriter that runs after IoMessage_parser has built
a flat message chain. Io's parser does not know about operators; every
`a + b * c` starts life as the linear chain `a + b * c`. This file
walks that chain keeping a stack of partially-built Levels (NEW / ARG
/ ATTACH / UNUSED), and rewrites higher-precedence operators into
arg-messages of lower-precedence operators so `a + b * c` becomes
`a +(b *(c))` — i.e. normal message form.

Two tables (IoState_createOperatorTable, IoState_createAssignOperator
Table) live in the VM's OperatorTable and can be customised from Io.
Assignment operators (:=, =, ::=) are special-cased to rewrite
`a := b` into `setSlot("a", b)` (or setSlotWithType for types whose
name starts with an uppercase letter). IoMessage_opShuffle is the
Io-visible entry; IoMessage_opShuffle_ is the C-only trigger invoked
by IoMessage_newFromText_labelSymbol_.
*/

#include "IoMessage_opShuffle.h"
#include "IoMap.h"
#include "IoObject.h"
#include "IoNumber.h"
#include "List.h"
#include "PHash.h"
#include <ctype.h>

#define DATA(self) ((IoMessageData *)IoObject_dataPointer(self))

#define IO_OP_MAX_LEVEL 32

/*cdoc Message IoState_createOperatorTable(state)
Builds the default binary operator precedence table as an IoMap of
symbol-&gt;number. Lower numbers bind tighter; `**` (exponent, level 1)
is tightest, assignment ops (level 13) and `return` (level 14) the
loosest. The returned map is stored under OperatorTable operators and
can be mutated from Io via the public OperatorTable API.
*/
IoMap *IoState_createOperatorTable(IoState *state) {
    typedef struct OpTable {
        char *symbol;
        int precedence;
    } OpTable;

    OpTable ops[] = {
        {"@", 0},       {"@@", 0},  {"?", 0},

        {"**", 1},

        {"*", 2},       {"/", 2},   {"%", 2},

        {"+", 3},       {"-", 3},

        {"<<", 4},      {">>", 4},

        {">", 5},       {"<", 5},   {"<=", 5},  {">=", 5},

        {"==", 6},      {"!=", 6},

        {"&", 7},

        {"^", 8},

        {"|", 9},

        {"and", 10},    {"&&", 10},

        {"or", 11},     {"||", 11},

        {"..", 12},

        {"+=", 13},     {"-=", 13}, {"*=", 13}, {"/=", 13},  {"%=", 13},
        {"&=", 13},     {"^=", 13}, {"|=", 13}, {"<<=", 13}, {">>=", 13},

        {"return", 14},

        {NULL, 0},
    };

    IoMap *self = IoMap_new(state);
    OpTable *op = ops;

    while (op->symbol) {
        IoMap_rawAtPut(self, IOSYMBOL(op->symbol), IONUMBER(op->precedence));
        op++;
    }

    return self;
}

/*cdoc Message IoState_createAssignOperatorTable(state)
Builds the default assign-operator -&gt; method-name table: `:=` becomes
setSlot, `=` becomes updateSlot, `::=` becomes newSlot. Levels_attach
consults this before the binary-op table; matching entries trigger
the `a := b` -&gt; `setSlot("a", b)` rewrite.
*/
IoMap *IoState_createAssignOperatorTable(IoState *state) {
    IoMap *self = IoMap_new(state);

    IoMap_rawAtPut(self, IOSYMBOL(":="), IOSYMBOL("setSlot"));
    IoMap_rawAtPut(self, IOSYMBOL("="), IOSYMBOL("updateSlot"));
    IoMap_rawAtPut(self, IOSYMBOL("::="), IOSYMBOL("newSlot"));

    return self;
}

enum LevelType { ATTACH, ARG, NEW, UNUSED };

typedef struct {
    IoMessage *message;
    enum LevelType type;
    int precedence;
} Level;

void Level_finish(Level *self);
void Level_attachAndReplace(Level *self, IoMessage *msg);
void Level_setAwaitingFirstArg(Level *self, IoMessage *msg, int precedence);

typedef struct {
    Level pool[IO_OP_MAX_LEVEL];
    int currentLevel;

    List *stack;
    IoMap *operatorTable;
    IoMap *assignOperatorTable;
} Levels;

/*cdoc Message Levels_reset(self)
Returns the Levels pool to the state expected at the start of a fresh
expression: every slot UNUSED except level 0, which is a NEW-type
sentinel with the maximum precedence so any operator binds tighter.
Called between top-level expressions by Levels_nextMessage.
*/
void Levels_reset(Levels *self) {
    int i;
    self->currentLevel = 1;

    for (i = 0; i < IO_OP_MAX_LEVEL; i++) {
        Level *level = &self->pool[i];
        level->type = UNUSED;
    }

    {
        Level *level = &self->pool[0];
        level->message = NULL;
        level->type = NEW;
        level->precedence = IO_OP_MAX_LEVEL;
    }

    List_removeAll(self->stack);
    List_append_(self->stack, &self->pool[0]);
}

// --- Levels ----------------------------------------------------------

/*cdoc Message getOpTable(self, slotName, create)
Fetches the named operator map slot from an OperatorTable object,
lazily creating and installing one via the supplied factory if it's
missing. Used to resolve both the precedence map and the
assign-operator map with a single helper.
*/
IoMap *getOpTable(IoObject *self, const char *slotName,
                  IoMap *create(IoState *state)) {
    IoSymbol *symbol = IoState_symbolWithCString_(IOSTATE, slotName);
    IoObject *operators = IoObject_rawGetSlot_(self, symbol);

    if (operators && ISMAP(operators)) {
        return operators;
    } else {
        /*
        Not strictly correct as if the message has its own empty
        OperatorTable slot, we'll create one for it instead of using
        Core Message OperatorTable operators. Oh well.
        */

        IoMap *result = create(IOSTATE);
        IoObject_setSlot_to_(self, symbol, result);
        return result;
    }
}

/*cdoc Message Levels_new(msg)
Allocates a fresh Levels and binds it to the OperatorTable object
reachable from `msg` (falling back to Core's OperatorTable, and
creating one from scratch if neither has one). The two maps are then
cached on the Levels so Levels_attach can do constant-time precedence
lookups as it walks the chain.
*/
Levels *Levels_new(IoMessage *msg) {
    Levels *self = io_calloc(1, sizeof(Levels));

    IoState *state = IoObject_state(msg);
    IoSymbol *operatorTableSymbol =
        IoState_symbolWithCString_(state, "OperatorTable");

    /* Be ultra flexible, and try to use the first message's operator table. */
    IoObject *opTable = IoObject_rawGetSlot_(msg, operatorTableSymbol);

    // Otherwise, use Core OperatorTable, and if that does not exist, create it.
    if (opTable == NULL) {
        /*
        There is a chance the message didn't have it, but the core did---due
        to the Core not being part of the message's protos. Use Core
        Message's OperatorTable
        */
        opTable = IoObject_rawGetSlot_(state->core, operatorTableSymbol);

        // If Core does not have an OperatorTable, then create it.
        if (opTable == NULL) {
            opTable = IoObject_new(state);
            IoObject_setSlot_to_(state->core, operatorTableSymbol, opTable);
            IoObject_setSlot_to_(
                opTable,
                IoState_symbolWithCString_(state, "precedenceLevelCount"),
                IoState_numberWithDouble_(state, IO_OP_MAX_LEVEL));
        }
    }

    self->operatorTable =
        getOpTable(opTable, "operators", IoState_createOperatorTable);
    self->assignOperatorTable = getOpTable(opTable, "assignOperators",
                                           IoState_createAssignOperatorTable);

    self->stack = List_new();
    Levels_reset(self);
    return self;
}

/*cdoc Message Levels_free(self)
Releases the stack List and the Levels struct. The Level pool is
inline so it needs no separate free.
*/
void Levels_free(Levels *self) {
    List_free(self->stack);
    io_free(self);
}

Level *Levels_currentLevel(Levels *self) { return List_top(self->stack); }

/*cdoc Message Levels_popDownTo(self, targetLevel)
Pops Levels off the stack until the top's precedence is strictly
greater than targetLevel, or until we hit a pending ARG level (which
is never popped implicitly — its consumer must supply the next token).
Level_finish runs on each popped level to wire up the completed
sub-tree.
*/
void Levels_popDownTo(Levels *self, int targetLevel) {
    Level *level;

    while (level = List_top(self->stack),
           level->precedence <= targetLevel && level->type != ARG) {
        Level_finish(List_pop(self->stack));
        self->currentLevel--;
    }
}

/*cdoc Message Levels_attachToTopAndPush(self, msg, precedence)
Attaches `msg` (an operator) onto the current top level, then pushes
a new ARG-type level awaiting the operator's right-hand side at the
given precedence. This is how `a + b * c` pushes a `*` level above
the `+` level so `b` and `c` get grouped first. Errors out if the
IO_OP_MAX_LEVEL-deep pool would overflow.
*/
void Levels_attachToTopAndPush(Levels *self, IoMessage *msg, int precedence) {
    Level *level = NULL;
    {
        Level *top = List_top(self->stack);
        Level_attachAndReplace(top, msg);
    }

    {
        // TODO: Check for overflow of the pool.
        if (self->currentLevel >= IO_OP_MAX_LEVEL) {
            IoState_error_(IoObject_state(msg), NULL,
                           "compile error: Overflowed operator stack. Only %d "
                           "levels of operators currently supported.",
                           IO_OP_MAX_LEVEL - 1);
        }

        level = &self->pool[self->currentLevel++];
        Level_setAwaitingFirstArg(level, msg, precedence);
        List_append_(self->stack, level);
    }
}

/*cdoc Message Level_finish(self)
Closes a Level as it is popped: severs the message's `next` link (the
sub-tree is now a complete operator subexpression) and flattens any
synthetic single-arg "()" wrapper that was inserted earlier to imitate
C-style grouping. Finally marks the slot UNUSED so Levels_reset's next
sweep sees a clean pool.
*/
void Level_finish(Level *self) {
    if (self->message) {
        IoMessage_rawSetNext_(self->message, NULL);

        // Remove extra () we added in for operators, but do not need any more
        if (IoMessage_argCount(self->message) == 1) {
            IoMessage *arg = IoMessage_rawArgAt_(self->message, 0);

            if (IoSeq_rawSize(IoMessage_name(arg)) == 0 &&
                IoMessage_argCount(arg) == 1 &&
                IoMessage_rawNext(arg) == NULL) {
                List_copy_(IoMessage_rawArgList(self->message),
                           IoMessage_rawArgList(arg));
                List_removeAll(IoMessage_rawArgList(arg));
            }
        }
    }

    self->type = UNUSED;
}

/*cdoc Message Level_attach(self, msg)
Wires `msg` into the current level according to its type: ATTACH links
via `next`, ARG appends as an argument to the level's message, NEW
adopts it as the level's message, UNUSED is a no-op. Used as the
primitive by Level_attachAndReplace and the ARG-push path.
*/
void Level_attach(Level *self, IoMessage *msg) {
    switch (self->type) {
    case ATTACH:
        IoMessage_rawSetNext_(self->message, msg);
        break;

    case ARG:
        IoMessage_addArg_(self->message, msg);
        break;

    case NEW:
        self->message = msg;
        break;

    case UNUSED:
        break;
    }
}

/*cdoc Message Level_attachAndReplace(self, msg)
Combines Level_attach with a state transition to ATTACH so subsequent
messages chain via `next`. The canonical "ordinary message" update for
a Level as the shuffler walks a chain of non-operator messages.
*/
void Level_attachAndReplace(Level *self, IoMessage *msg) {
    Level_attach(self, msg);
    self->type = ATTACH;
    self->message = msg;
}

/*cdoc Message Level_setAwaitingFirstArg(self, msg, precedence)
Puts a freshly-pushed Level into ARG mode so the next shuffled message
is appended as the first argument of `msg` (the operator). The recorded
precedence is what Levels_popDownTo compares against to unwind.
*/
void Level_setAwaitingFirstArg(Level *self, IoMessage *msg, int precedence) {
    self->type = ARG;
    self->message = msg;
    self->precedence = precedence;
}

void Level_setAlreadyHasArgs(Level *self, IoMessage *msg) {
    self->type = ATTACH;
    self->message = msg;
}

/*cdoc Message Levels_levelForOp(self, messageName, messageSymbol, msg)
Looks up the precedence for `messageSymbol` in the binary operator map.
Returns -1 if the symbol isn't an operator. Validates that the mapped
value is a Number in [0, IO_OP_MAX_LEVEL) and raises a compile error
otherwise — user code can edit OperatorTable operators from Io so
defensive checking is required.
*/
int Levels_levelForOp(Levels *self, char *messageName, IoSymbol *messageSymbol,
                      IoMessage *msg) {
    IoObject *value = IoMap_rawAt(self->operatorTable, messageSymbol);

    if (!value) {
        return -1;
    }

    if (ISNUMBER(value)) {
        int precedence = IoNumber_asInt((IoNumber *)value);
        if (precedence < 0 || precedence >= IO_OP_MAX_LEVEL) {
            IoState_error_(IoObject_state(msg), msg,
                           "compile error: Precedence for operators must be "
                           "between 0 and %d. Precedence was %d.",
                           IO_OP_MAX_LEVEL - 1, precedence);
        }

        return precedence;
    } else {
        IoState_error_(
            IoObject_state(msg), msg,
            "compile error: Value for '%s' in Message OperatorTable operators "
            "is not a number. Values in the OperatorTable operators are "
            "numbers which indicate the precedence of the operator.",
            messageName);
        return -1; // The C compiler does not know that IoState_error_() will
                   // never return.
    }
}

/*cdoc Message Levels_isAssignOperator(self, operator)
True if the symbol is a key in the assign-operator map (i.e. `:=`,
`=`, or `::=` by default). Levels_attach uses this to branch into
the assign-rewrite path before trying precedence-based handling.
*/
int Levels_isAssignOperator(Levels *self, IoSymbol *operator) {
    return IoMap_rawAt(self->assignOperatorTable, operator) != NULL;
}

/*cdoc Message Levels_nameForAssignOperator(self, state, operator, slotName, msg)
Resolves an assign operator like `:=` to the slot-method name to emit
(e.g. setSlot). Special-cases `:=` with an uppercase slot-name first
character to return setSlotWithType so `MyType := X` becomes
setSlotWithType("MyType", X). Raises a compile error if the configured
value is not a Symbol.
*/
IoSymbol *Levels_nameForAssignOperator(Levels *self, IoState *state,
                                       IoSymbol *operator, IoSymbol * slotName,
                                       IoMessage *msg) {
    IoObject *value = IoMap_rawAt(self->assignOperatorTable, operator);
    const char *operatorString = CSTRING(operator);

    if (value != NULL && ISSYMBOL(value)) {
        if (strcmp(operatorString, ":=") == 0 &&
            isupper(CSTRING(slotName)[0])) {
            return state->setSlotWithTypeSymbol;
        } else {
            return value;
        }
    } else {
        const char *error = "compile error: Value for '%s' in Message "
                            "OperatorTable assignOperators is not a symbol. "
                            "Values in the OperatorTable assignOperators are "
                            "symbols which are the name of the operator.";

        IoState_error_(IoObject_state(msg), msg, error, operatorString);
        return NULL; // To keep the compiler happy.
    }
}

/*cdoc Message Levels_attach(self, msg, expressions)
Core shuffler step: decides what to do with the next message in the
chain. Three branches: (1) assign operator — rewrite the attaching
message into setSlot/updateSlot/newSlot with the slot name quoted as
a cached-result argument, then splice the value expression into the
new argument list, pushing the tail onto `expressions` for later; (2)
end-of-line `;` — pop down to the lowest precedence so a new statement
starts fresh; (3) non-assign operator — wrap any preset args in a
synthetic "()" grouping so `a +(b,c)` parses sanely, pop higher-or-
equal precedence levels, then push a new ARG level. Otherwise attach
as a normal message.
*/
void Levels_attach(Levels *self, IoMessage *msg, List *expressions) {
    IoState *state = IoObject_state(msg);
    IoSymbol *messageSymbol = IoMessage_name(msg);
    const char *messageName = CSTRING(messageSymbol);
    int precedence =
        Levels_levelForOp(self, (char *)messageName, messageSymbol, msg);

    int msgArgCount = IoMessage_argCount(msg);

    /*
    // Expression:  o a := b c ; d
    // becomes:     o setSlot("a", b c) ; d
    //
    // a      attaching
    // :=     msg
    // b c    msg->next
//
    */

    if (Levels_isAssignOperator(self, messageSymbol)) {
        Level *currentLevel = Levels_currentLevel(self);
        IoMessage *attaching = currentLevel->message;
        IoSymbol *setSlotName;

        if (attaching == NULL) /* := b ; */ {
            // Could be handled as, message(:= 42) -> setSlot(nil, 42)
            const char *text =
                "compile error: %s requires an expression to its left.";
            IoState_error_(state, msg, text, messageName);
            return;
        }

        if (IoMessage_argCount(attaching) > 0) { // a(1,2,3) := b ;
            // Expression: target msgName(v1, v1, v3) assignOp   v4    ; rest
            //                    ^^^^^^^^^^^^^^^^^^^ ^^^^^^^^  ^^^^^  ^^^^^^
            //                      slotNameMessage     msg      val    rest
            // becomes:    target assignOpName(msgName(v1, v2, v3), v4) ; rest

            setSlotName = Levels_nameForAssignOperator(
                self, state, messageSymbol, NULL, msg);

            IoMessage *slotNameMessageCopy = IoMessage_deepCopyOf_(attaching);
            IoMessage_rawSetNext_(slotNameMessageCopy, NULL);

            IoMessage *slotNameMessage = attaching;
            DATA(slotNameMessage)->name = setSlotName;
            DATA(slotNameMessage)->args = List_new();
            IoMessage_addArg_(slotNameMessage, slotNameMessageCopy);

            IoMessage *value = IoMessage_deepCopyOf_(DATA(msg)->next);
            IoMessage_rawSetNext_(value, NULL);
            // Step forward twice to the "next" part of the expression,
            // which may be NULL, and make a copy.
            IoMessage *rest = DATA(msg)->next;
            if (rest) {
	        rest = DATA(rest)->next;
            }
            if (rest) {
                rest = IoMessage_deepCopyOf_(rest);
            }
            IoMessage_rawSetNext_(slotNameMessage, rest);
            IoMessage_addArg_(slotNameMessage, value);

            /* printf("slotNameMessage: %s\n",
             * CSTRING(IoObject_asString_(slotNameMessage, msg))); */
            /* printf("rest: %s\n", CSTRING(IoObject_asString_(rest, msg))); */
            return;
        }

        if (msgArgCount > 1) { // setSlot("a") :=(b, c, d) e ;
            IoState_error_(state, msg,
                           "compile error: Assign operator passed multiple "
                           "arguments, e.g., a := (b, c).",
                           messageName);
            return;
        }

        {
            // a := b ;
            IoSymbol *slotName = DATA(attaching)->name;
            IoSymbol *quotedSlotName =
                IoSeq_newSymbolWithFormat_(state, "\"%s\"", CSTRING(slotName));
            IoMessage *slotNameMessage = IoMessage_newWithName_returnsValue_(
                state, quotedSlotName, slotName);

            IoMessage_rawCopySourceLocation(slotNameMessage, attaching);

            // a := b ;  ->  a("a") := b ;
            IoMessage_addArg_(attaching, slotNameMessage);

            setSlotName = Levels_nameForAssignOperator(
                self, state, messageSymbol, slotName, msg);
        }

        // a("a") := b ;  ->  setSlot("a") := b ;
        DATA(attaching)->name = IoObject_addingRef_(attaching, setSlotName);

        currentLevel->type = ATTACH;

        if (msgArgCount > 0) // setSlot("a") :=(b c) d e ;
        {
            // b c
            IoMessage *arg = IoMessage_rawArgAt_(msg, 0);

            if (DATA(msg)->next == NULL ||
                IoMessage_rawIsEOL(DATA(msg)->next)) {
                IoMessage_addArg_(attaching, arg);
            } else {
                // ()
                IoMessage *foo = IoMessage_newWithName_(
                    state, IoState_symbolWithCString_(state, ""));

                IoMessage_rawCopySourceLocation(foo, attaching);

                // ()  ->  (b c)
                IoMessage_addArg_(foo, arg);

                // (b c)  ->  (b c) d e ;
                IoMessage_rawSetNext_(foo, DATA(msg)->next);

                // setSlot("a") :=(b c) d e ;  ->  setSlot("a", (b c) d e ;)
                // :=(b c) d e ;
                IoMessage_addArg_(attaching, foo);
            }
        } else // setSlot("a") := b ;
        {
            // setSlot("a") := or setSlot("a") := ;
            IoMessage *mn = DATA(msg)->next;
            IoSymbol *name = mn ? DATA(mn)->name : NULL;
            IoSymbol *semi = IoObject_state(msg)->semicolonSymbol;

            // if (mn == NULL || IoMessage_rawIsEOL(mn))
            if (mn == NULL || name == semi) {
                IoState_error_(state, msg,
                               "compile error: %s must be followed by a value.",
                               messageName);
            }

            // setSlot("a") := b c ;  ->  setSlot("a", b c ;) := b c ;
            IoMessage_addArg_(attaching, DATA(msg)->next);
        }

        // process the value (b c d) later  (setSlot("a", b c d) := b c d ;)
        if (DATA(msg)->next != NULL && !IoMessage_rawIsEOL(DATA(msg)->next)) {
            List_push_(expressions, DATA(msg)->next);
        }

        {
            IoMessage *last = msg;
            while (DATA(last)->next != NULL &&
                   !IoMessage_rawIsEOL(DATA(last)->next)) {
                last = DATA(last)->next;
            }

            IoMessage_rawSetNext_(attaching, DATA(last)->next);

            // Continue processing in IoMessage_opShuffle loop
            IoMessage_rawSetNext_(msg, DATA(last)->next);

            if (last != msg) {
                IoMessage_rawSetNext_(last, NULL);
            }
        }

        // make sure b in 1 := b gets executed
        IoMessage_rawSetCachedResult_(attaching, NULL);
    } else if (IoMessage_rawIsEOL(msg)) {
        Levels_popDownTo(self, IO_OP_MAX_LEVEL - 1);
        Level_attachAndReplace(Levels_currentLevel(self), msg);
    } else if (precedence != -1) // is an operator
    {
        if (msgArgCount > 0) {
            // move arguments off to their own message to make () after
            // operators behave like Cs grouping ()
            IoMessage *brackets = IoMessage_newWithName_(
                state, IoState_symbolWithCString_(state, ""));

            IoMessage_rawCopySourceLocation(brackets, msg);

            List_copy_(IoMessage_rawArgList(brackets),
                       IoMessage_rawArgList(msg));
            List_removeAll(IoMessage_rawArgList(msg));

            // Insert the brackets message between msg and its next message
            IoMessage_rawSetNext_(brackets, DATA(msg)->next);
            IoMessage_rawSetNext_(msg, brackets);
        }

        Levels_popDownTo(self, precedence);
        Levels_attachToTopAndPush(self, msg, precedence);
    } else {
        Level_attachAndReplace(Levels_currentLevel(self), msg);
    }
}

/*cdoc Message Levels_nextMessage(self)
Finishes the current expression: pops every Level on the stack through
Level_finish (wiring up any remaining open operator sub-trees), then
resets the pool for the next top-level expression. Called by
IoMessage_opShuffle between expressions popped from its worklist.
*/
void Levels_nextMessage(Levels *self) {
    Level *level;

    while ((level = List_pop(self->stack))) {
        Level_finish(level);
    }

    Levels_reset(self);
}

/*cdoc Message IoMessage_opShuffle_(self)
C-side trigger used by IoMessage_newFromText_labelSymbol_ to kick off
operator shuffling via the Io message `opShuffle` sent to `self`. This
indirection lets Io code override opShuffle; the guard against
noShufflingSymbol skips shuffling for synthesised messages whose name
signals that they already arrived in normal form.
*/
void IoMessage_opShuffle_(IoMessage *self) {
    if (IoObject_rawGetSlot_(self, IOSTATE->opShuffleSymbol) &&
        IoMessage_name(self) != IOSTATE->noShufflingSymbol) {
        IoMessage_locals_performOn_(IOSTATE->opShuffleMessage, IOSTATE->lobby,
                                    self);
    }
}

/*cdoc Message IoMessage_opShuffle(self, locals, m)
Io-visible entry point. Walks the message tree iteratively using an
explicit `expressions` worklist instead of C recursion — each
Levels_attach call may push sub-expressions (the tails of assign
rewrites, plus each message's args) for later processing. Returns
self with the tree rewritten in place.
*/
IoMessage *IoMessage_opShuffle(IoMessage *self, IoObject *locals,
                               IoMessage *m) {
    Levels *levels = Levels_new(self);
    List *expressions = List_new();

    List_push_(expressions, self);

    while (List_size(expressions) >= 1) {
        IoMessage *n = List_pop(expressions);

        do {
            Levels_attach(levels, n, expressions);
            List_appendSeq_(expressions, DATA(n)->args);
        } while ((n = DATA(n)->next));

        Levels_nextMessage(levels);
    }

    List_free(expressions);
    Levels_free(levels);

    return self;
}
