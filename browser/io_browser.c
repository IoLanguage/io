// Browser entry point for Io VM
// Compiled with -mexec-model=reactor to export functions for JS to call.
// Output is captured via printCallback into a growable buffer.

#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "UArray.h"
#include "io_js_bridge.h"
#include "io_future.h"
#include "IoBigInt.h"
#include "IoEvalFrame.h"
#include "IoCoroutine.h"
#include "IoList.h"
#include "IoNumber.h"
#include "IoMessage.h"
#include "Stack.h"
#include "Date.h"
#include <stdlib.h>
#include <string.h>

// Import from js module (defined in io_js_bridge.c but we need it here for init)
__attribute__((import_module("js"), import_name("js_get_global")))
extern int js_get_global(void);

static IoState *state = NULL;
static const char *scheduler_io_source;

// Exposed for io_js_bridge.c (io_send, io_get_lobby_handle)
IoState *io_bridge_state = NULL;


// Input buffer: JS writes code here, then calls io_eval_input()
#define INPUT_BUF_SIZE (64 * 1024)
static char input_buf[INPUT_BUF_SIZE];

// Exported: pointer to input buffer (JS writes UTF-8 code string here)
__attribute__((export_name("io_get_input_buf")))
char *io_get_input_buf(void) {
	return input_buf;
}

// Exported: size of input buffer
__attribute__((export_name("io_get_input_buf_size")))
size_t io_get_input_buf_size(void) {
	return INPUT_BUF_SIZE;
}

// Output capture buffer
static char *output_buf = NULL;
static size_t output_len = 0;
static size_t output_cap = 0;

static void output_append(const char *data, size_t len) {
	if (output_len + len + 1 > output_cap) {
		output_cap = (output_len + len + 1) * 2;
		if (output_cap < 4096) output_cap = 4096;
		output_buf = realloc(output_buf, output_cap);
	}
	memcpy(output_buf + output_len, data, len);
	output_len += len;
	output_buf[output_len] = '\0';
}

static void output_clear(void) {
	output_len = 0;
	if (output_buf) output_buf[0] = '\0';
}

// printCallback: redirect all Io output to our buffer
static void browser_print_callback(void *context, const UArray *ba) {
	(void)context;
	output_append((const char *)UArray_bytes(ba), UArray_size(ba));
}

// Set when an uncaught exception reaches the top level during an eval.
// IoState_tryToPerform clears state->errorRaised after reporting, so this
// flag is the only way do_eval can tell the input failed.
static int exception_raised = 0;

// Exception callback: capture exception text
static void browser_exception_callback(void *context, IoObject *coroutine) {
	(void)context;
	exception_raised = 1;
	// Use the default backtrace printing — it goes through printCallback
	IoCoroutine_rawPrintBackTrace(coroutine);
}

void IoAddonsInit(IoObject *context) { (void)context; }

static void host_poll(IoState *s);   // see "Time slices and interrupt"

// Scheduler hostCanSwitch — false inside a nested C eval loop. A scheduler
// tick runs its eval loop at depth 1; anything deeper is a C function that
// is evaluating Io code itself and expects it to finish before it returns.
static IoObject *IoObject_hostCanSwitch(IoObject *self, IoObject *locals, IoMessage *m) {
	(void)locals; (void)m;
	return IOSTATE->nestedEvalDepth <= 1 ? IOSTATE->ioTrue : IOSTATE->ioFalse;
}

// A finished coroutine keeps whatever its last frames left on its retain
// stack, and that includes other coroutines (the ones a scheduler resumed).
// Since each coroutine also points at the one that started it, finished
// coroutines can keep each other alive in a chain as long as history. So
// drop a coroutine's retain stack once it is known to be finished.
static void release_finished(IoObject *coro) {
	if (!coro || !ISCOROUTINE(coro)) return;
	IoCoroutineData *data = (IoCoroutineData *)IoObject_dataPointer(coro);
	IoState *s = (IoState *)IoObject_state(coro);
	if (data->hasFinished && coro != (IoObject *)s->currentCoroutine) {
		Stack_clear(data->ioStack);
	}
}

// Scheduler hostRelease(aCoroutine) — see release_finished
static IoObject *IoObject_hostRelease(IoObject *self, IoObject *locals, IoMessage *m) {
	IoObject *coro = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (!IOSTATE->errorRaised) release_finished(coro);
	return self;
}

// jsfunction CFunction (defined in io_js_bridge.c)
extern IoObject *IoObject_jsfunction(IoObject *self, IoObject *locals, IoMessage *m);

// Exported: initialize the Io VM. Call once on startup.
__attribute__((export_name("io_init")))
int io_init(void) {
	if (state) return 0; // already initialized

	state = IoState_new();
	IoState_init(state);
	state->hostPollCallback = host_poll;

	// Wire output capture
	IoState_printCallback_(state, browser_print_callback);
	IoState_exceptionCallback_(state, browser_exception_callback);

	// Set minimal args so System args doesn't crash
	const char *argv[] = {"io"};
	IoState_argc_argv_(state, 1, argv);

	// Register JS bridge (must be before Element so Element can inherit forward)
	IoObject *jsObjProto = IoJSObject_proto(state);
	int globalHandle = js_get_global();
	IoObject *jsSingleton = IoJSObject_newWithHandle_(state, globalHandle);
	IoObject_setSlot_to_(state->core,
		IoState_symbolWithCString_(state, "JS"), jsSingleton);
	IoObject_setSlot_to_(state->core,
		IoState_symbolWithCString_(state, "JSObject"), jsObjProto);

	// Register IoFuture proto. JSFuture names it so the scheduler source
	// below can add its Io-side methods.
	IoObject_setSlot_to_(state->core,
		IoState_symbolWithCString_(state, "JSFuture"), IoFuture_proto(state));

	// Register IoBigInt proto
	IoObject *bigIntProto = IoBigInt_proto(state);
	IoObject_setSlot_to_(state->core,
		IoState_symbolWithCString_(state, "BigInt"), bigIntProto);

	// Register jsfunction on Lobby
	{
		IoMethodTable jsfuncMethods[] = {
			{"jsfunction", IoObject_jsfunction},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(state->lobby, jsfuncMethods);
	}

	// Expose state for JS bridge exports
	io_bridge_state = state;

	// Scheduler (see below): one C primitive, the rest is Io
	{
		IoMethodTable schedulerMethods[] = {
			{"hostCanSwitch", IoObject_hostCanSwitch},
			{"hostRelease", IoObject_hostRelease},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(
			IoObject_getSlot_(state->lobby, IoState_symbolWithCString_(state, "Scheduler")),
			schedulerMethods);
	}
	IoState_doCString_(state, scheduler_io_source);

	output_clear();
	return 0;
}

// ---- Scheduler ----
//
// JS owns the run loop, so Io never blocks in the browser. Every entry from
// JS runs one "tick": a short-lived scheduler coroutine (Scheduler hostRun,
// below) that resumes runnable coroutines until none are left or a time
// budget runs out, then returns to JS with the time of the next timer. JS
// calls io_tick again from a timer, or when a Promise settles.
//
// An eval is a coroutine too (Scheduler hostEval). If it parks — on `wait`,
// a JS Promise, an actor future — io_eval_input returns 2 and a later tick
// finishes it.

static const char *scheduler_io_source =
	"Scheduler do(\n"
	"	hostBudget := 0.01\n"       // seconds of Io work per tick
	"	hostEvalCoro := nil\n"      // the eval in progress, set from C
	"	hostStart := nil\n"         // a coroutine to run first this tick, set from C
	"	hostAwaiters := List clone\n"   // coroutines parked on JS Promises
	"\n"
	"	hostRun := method(\n"
	"		me := Coroutine currentCoroutine\n"
	"		first := hostStart\n"
	"		if(first, yieldingCoros atInsert(0, first); self hostStart = nil)\n"
	"		deadline := Date clone now asNumber + hostBudget\n"
	"		loop(\n"
	"			yieldingCoros remove(me)\n"
	"			wakeExpiredTimers\n"
	"			next := nil\n"
	"			while(next isNil and(yieldingCoros isEmpty not),\n"
	"				c := yieldingCoros removeFirst\n"
	"				if(c isFinished not, next = c)\n"
	"			)\n"
	"			if(next isNil, break)\n"
	// Staying on the run queue means a coroutine that yields or pauses
	// always finds something to switch to, and comes back here.
	"			yieldingCoros append(me)\n"
	"			next resume\n"
	// Return to JS as soon as the JS call this tick was started for has
	// finished or parked, or the eval in progress has finished (whichever
	// coroutine resumed it), leaving the rest for later ticks.
	"			if(next == first and(next isFinished or(next != hostEvalCoro)), break)\n"
	"			if(hostEvalCoro and(hostEvalCoro isFinished), break)\n"
	"			if(next isFinished,\n"
	"				if(next parentCoroutine == me and(next exception isNil not), next exception showStack)\n"
	"				hostRelease(next)\n"
	"			)\n"
	"			if(Date clone now asNumber > deadline, break)\n"
	"		)\n"
	"		yieldingCoros remove(me)\n"
	// Result: seconds until the next tick is needed. 0 = now. -1 = not
	// until a JS Promise settles. -2 = nothing in Io is due to run at all.
	"		if(yieldingCoros isEmpty not, return 0)\n"
	// An interrupted coroutine leaves its timer behind, and never takes
	// itself off the list of awaiters
	"		timers selectInPlace(t, t at(1) isFinished not)\n"
	"		if(timers isEmpty,\n"
	"			hostAwaiters selectInPlace(c, c isFinished not)\n"
	"			return if(hostAwaiters isEmpty, -2, -1)\n"
	"		)\n"
	"		(timers first at(0) - Date clone now asNumber) max(0)\n"
	"	)\n"
	"\n"
	"	hostEval := method(code,\n"
	"		r := Lobby doString(code, \"IoState_doCString\")\n"
	"		if(getSlot(\"r\") isNil, nil, getSlot(\"r\") asString)\n"
	"	)\n"
	")\n"
	// Switching coroutines only works from frames the scheduler's own eval
	// loop is running. Inside a nested C eval loop (a JS callback made during
	// an Io call, a comparison block run by sortBy) the switch would leave
	// that C call half-finished, so make it an error instead of a hang.
	"Coroutine hostResume := Coroutine getSlot(\"resume\")\n"
	"Coroutine resume := method(\n"
	"	if(Scheduler hostCanSwitch not,\n"
	"		Exception raise(\"can't wait or switch coroutines here: this code was called by something that needs its result immediately (a JS callback made during an Io call, or a block run by a primitive such as sortBy)\")\n"
	"	)\n"
	"	self hostResume\n"
	")\n"
	// yield and pause take coroutines off the run queue and then switch to
	// one. A time slice must not end partway through: the coroutine would go
	// back on the queue, be run again, and finish its yield with no entry of
	// its own left, parked for good (or return from pause without pausing).
	// hostNoSlice makes the slice wait until they are done.
	"Coroutine hostNoSlice := false\n"
	"Coroutine hostYield := Coroutine getSlot(\"yield\")\n"
	"Coroutine yield := method(\n"
	"	c := Coroutine currentCoroutine\n"
	"	c hostNoSlice := true\n"
	"	self hostYield\n"
	"	c hostNoSlice := false\n"
	"	self\n"
	")\n"
	"Coroutine hostPause := Coroutine getSlot(\"pause\")\n"
	"Coroutine pause := method(\n"
	"	c := Coroutine currentCoroutine\n"
	"	c hostNoSlice := true\n"
	"	self hostPause\n"
	"	c hostNoSlice := false\n"
	"	self\n"
	")\n"
	// JS Promises: await parks the coroutine until the Promise settles.
	"JSFuture do(\n"
	"	await := method(\n"
	"		if(isReady not,\n"
	"			if(Scheduler hostCanSwitch not, Coroutine currentCoroutine resume)\n"   // raises
	"			Scheduler hostAwaiters append(Coroutine currentCoroutine)\n"
	"			while(isReady not,\n"
	"				if(self hasLocalSlot(\"waiters\") not, self waiters := List clone)\n"
	"				waiters appendIfAbsent(Coroutine currentCoroutine)\n"
	"				Coroutine currentCoroutine pause\n"
	"			)\n"
	"			Scheduler hostAwaiters remove(Coroutine currentCoroutine)\n"
	"		)\n"
	"		rawAwait\n"
	"	)\n"
	// Any other message waits for the value and is sent on to it. Only this
	// message: the eval loop carries on with the rest of the chain itself.
	"	forward := method(\n"
	"		v := self await\n"
	"		m := call message clone setNext(nil)\n"
	"		getSlot(\"v\") doMessage(m, call sender)\n"
	"	)\n"
	")\n";

#define CORO_DATA(c) ((IoCoroutineData *)IoObject_dataPointer(c))

static IoObject *evalCoro = NULL;   // eval in progress, or NULL
static size_t eval_output_len = 0;  // output bytes that belong to the last eval
static double next_wake = -1;       // seconds until the next tick; see hostRun
static int nothing_due = 0;         // last tick left nothing runnable, timed or awaited

static IoObject *scheduler_object(void) {
	return IoObject_getSlot_(state->lobby, IoState_symbolWithCString_(state, "Scheduler"));
}

// Put a coroutine back on the run queue (used when a JS Promise settles).
void io_browser_make_runnable(IoObject *coro) {
	IoObject *queue = IoObject_getSlot_(scheduler_object(),
		IoState_symbolWithCString_(state, "yieldingCoros"));
	if (!ISLIST(queue) || !ISCOROUTINE(coro)) return;
	IoList_rawRemove_(queue, coro);
	IoList_rawAppend_(queue, coro);
}

// ---- Time slices and interrupt ----
//
// Io code that never waits would otherwise run until it finishes, with the
// page frozen and no way for JS to stop it. With a time slice set, the eval
// loop's host poll parks whichever coroutine has used up the slice, where it
// stands, and returns to the tick's scheduler, which returns to JS. The
// coroutine is back on the run queue, so the next tick carries it on.
//
// io_interrupt asks for the eval in progress to be stopped: the next time
// its coroutine runs, the poll raises an error in it, and keeps raising it
// up through any `try` until the eval has ended.
//
// Background code (a coroutine that is not the eval) that never waits is
// sliced the same way, and nothing stops it unless JS asks. The poll notes
// when a background coroutine is parked after running for most of a slice
// without switching of its own accord: io_background_busy_ms reports how
// long that has been going on, and io_interrupt_background stops the next
// coroutine it happens to. Coroutines that wait (on a timer, a Promise, an
// actor message) or yield are never counted and never stopped.

static IoCoroutine *tick_sched = NULL;   // scheduler coroutine of the tick in progress
static int tick_can_slice = 0;           // this tick may park coroutines
static double slice_seconds = 0;         // 0 = no time slices
static double slice_end = 0;
static int interrupt_requested = 0;

static unsigned int run_resume_count = 0;   // coroutineResumeCount when last polled
static double run_since = 0;                // when the running coroutine was first seen since then
static double bg_busy_since = 0;            // start of the current run of busy slices
static double bg_busy_last = 0;             // the latest one; 0 = none
static double bg_interrupt_at = 0;          // when a background stop was asked for; 0 = not
static IoCoroutine *bg_stopping = NULL;     // background coroutine being stopped

// Busy slices further apart than this are separate episodes, and a
// background stop that finds nothing to stop for this long lapses.
static double bg_gap(void) {
	return slice_seconds * 4 + 0.25;
}

// The coroutine `top` is running in: itself, or the one a chain of `try`s
// (or anything else that runs a coroutine and waits for it) leads down to.
static IoCoroutine *running_coro_from(IoCoroutine *top) {
	IoCoroutine *c = top;
	for (;;) {
		if (c == IoState_currentCoroutine(state) && state->currentFrame) return c;
		IoEvalFrame *frame = CORO_DATA(c)->frameStack;
		if (!frame || FRAME_DATA(frame)->state != FRAME_STATE_CORO_WAIT_CHILD) return c;
		IoObject *child = FRAME_DATA(frame)->controlFlow.coroInfo.childCoroutine;
		if (!child || !ISCOROUTINE(child) || CORO_DATA(child)->hasFinished) return c;
		c = child;
	}
}

// The other way: the coroutine at the top of the chain `c` is running for.
static IoCoroutine *top_coro_of(IoCoroutine *c) {
	for (;;) {
		IoObject *parent = IoCoroutine_rawParentCoroutine(c);
		if (!parent || !ISCOROUTINE(parent) || parent == c) return c;
		IoEvalFrame *frame = CORO_DATA(parent)->frameStack;
		if (!frame || FRAME_DATA(frame)->state != FRAME_STATE_CORO_WAIT_CHILD) return c;
		if (FRAME_DATA(frame)->controlFlow.coroInfo.childCoroutine != (IoObject *)c) return c;
		c = parent;
	}
}

// The coroutine the eval in progress is running in.
static IoCoroutine *eval_running_coro(void) {
	return running_coro_from(evalCoro);
}

static void host_poll(IoState *s) {
	// While an interrupt is pending, look at every step: the eval's
	// coroutine may run only briefly before it parks itself again.
	if (interrupt_requested || bg_stopping) s->hostPollCountdown = 1;

	// Only in a tick's own eval loop. Deeper is a C function evaluating Io
	// code that it needs to finish.
	if (!tick_sched || s->nestedEvalDepth != 1) return;
	IoCoroutine *active = IoState_currentCoroutine(s);
	if (active == tick_sched) {
		bg_stopping = NULL;   // it has ended: the scheduler is running again
		return;
	}

	if (interrupt_requested && evalCoro && active == eval_running_coro()) {
		IoState_error_(s, FRAME_DATA(s->currentFrame)->message, "interrupted");
		return;
	}
	if (bg_stopping && active == running_coro_from(bg_stopping)) {
		IoState_error_(s, FRAME_DATA(s->currentFrame)->message, "interrupted");
		return;
	}

	if (!tick_can_slice || slice_seconds <= 0) return;
	double now = Date_SecondsFrom1970ToNow();
	if (s->coroutineResumeCount != run_resume_count) {
		// A coroutine has switched since the last poll, so this one hasn't
		// been running all that time
		run_resume_count = s->coroutineResumeCount;
		run_since = now;
	}
	if (now < slice_end) return;
	if (CORO_DATA(tick_sched)->frameStack == NULL) return;
	if (ISTRUE(IoObject_getSlot_(active, IoState_symbolWithCString_(s, "hostNoSlice")))) {
		s->hostPollCountdown = 8;   // partway through yield or pause: look again shortly
		return;
	}

	// Background code that ran for most of the slice without waiting or
	// yielding. (run_since is the first poll after it started, a little
	// late, hence "most".)
	if (now - run_since >= slice_seconds * 0.75 && !(evalCoro && active == eval_running_coro())) {
		if (bg_interrupt_at > 0 && now - bg_interrupt_at <= bg_gap()) {
			// Asked to stop it. Raised here and at every step until it ends.
			bg_interrupt_at = 0;
			bg_busy_last = 0;
			bg_stopping = top_coro_of(active);
			s->hostPollCountdown = 1;
			IoState_error_(s, FRAME_DATA(s->currentFrame)->message, "interrupted");
			return;
		}
		if (bg_busy_last == 0 || now - bg_busy_last > bg_gap()) bg_busy_since = now;
		bg_busy_last = now;
	}

	// Back on the run queue, behind anything else that is ready
	IoObject *queue = IoObject_getSlot_(scheduler_object(),
		IoState_symbolWithCString_(s, "yieldingCoros"));
	if (!ISLIST(queue)) return;
	IoList_rawRemove_(queue, active);
	IoList_rawAppend_(queue, active);

	// The scheduler is parked in hostRun's `next resume`, and carries on
	// from there. The frames of the coroutine being parked are untouched.
	IoCoroutine_saveState_(active, s);
	IoState_setCurrentCoroutine_(s, tick_sched);
	IoCoroutine_restoreState_(tick_sched, s);
}

// Exported: the longest Io code runs before the page gets a turn, in
// milliseconds. 0 (the default) means it runs until it waits or finishes.
__attribute__((export_name("io_set_time_slice_ms")))
void io_set_time_slice_ms(double ms) {
	slice_seconds = ms > 0 ? ms / 1000.0 : 0;
}

// Exported: stop the eval in progress with an "interrupted" error. Returns
// 1 if there is one, 0 if not. The error is raised when the eval next runs,
// so a tick has to follow.
__attribute__((export_name("io_interrupt")))
int io_interrupt(void) {
	if (!state || !evalCoro || CORO_DATA(evalCoro)->hasFinished) return 0;
	interrupt_requested = 1;
	state->hostPollCountdown = 0;
	// If it is waiting (on a timer, a Promise, another coroutine), run it
	if (!state->currentFrame) io_browser_make_runnable(eval_running_coro());
	return 1;
}

// Exported: how long background code (not the eval) has been running
// without waiting, in milliseconds: it has used up time slice after time
// slice. 0 if it isn't, or there are no time slices.
__attribute__((export_name("io_background_busy_ms")))
double io_background_busy_ms(void) {
	if (bg_busy_last == 0 || slice_seconds <= 0) return 0;
	double now = Date_SecondsFrom1970ToNow();
	if (now - bg_busy_last > bg_gap()) {
		bg_busy_last = 0;
		return 0;
	}
	double ms = (now - bg_busy_since) * 1000.0;
	return ms < 1 ? 1 : ms;
}

// Exported: stop the background coroutine that next uses up a time slice
// without waiting, with an "interrupted" error. Returns 1 if background
// code is busy now (see io_background_busy_ms), and 0, doing nothing, if
// not. Coroutines that wait are never stopped by this.
__attribute__((export_name("io_interrupt_background")))
int io_interrupt_background(void) {
	if (!state || io_background_busy_ms() == 0) return 0;
	bg_interrupt_at = Date_SecondsFrom1970ToNow();
	return 1;
}

// Run one scheduler tick. Must not be called while Io code is running.
// canSlice: coroutines may be parked when the time slice runs out.
static void run_scheduler(int canSlice) {
	IoObject *scheduler = scheduler_object();
	IoCoroutine *previous = IoState_currentCoroutine(state);

	// Kept alive by the caller's retain pool on the main coroutine's stack
	IoCoroutine *sched = IoCoroutine_new(state);
	IoCoroutine_rawSetRunTarget_(sched, scheduler);
	IoCoroutine_rawSetRunLocals_(sched, scheduler);
	IoCoroutine_rawSetRunMessage_(sched,
		IoMessage_newWithName_(state, IoState_symbolWithCString_(state, "hostRun")));

	IoState_setCurrentCoroutine_(state, sched);
	state->currentFrame = NULL;
	state->frameDepth = 0;
	state->currentIoStack = CORO_DATA(sched)->ioStack;

	IoEvalFrame *frame = IoState_pushFrame_(state);
	IoEvalFrameData *fd = FRAME_DATA(frame);
	fd->message = IoCoroutine_rawRunMessage(sched);
	fd->target = scheduler;
	fd->locals = scheduler;
	fd->cachedTarget = scheduler;
	fd->state = FRAME_STATE_START;

	tick_sched = sched;
	tick_can_slice = canSlice;
	slice_end = Date_SecondsFrom1970ToNow() + slice_seconds;
	state->hostPollCountdown = 0;

	IoObject *result = state->ioNil;
	state->nestedEvalDepth++;
	for (;;) {
		result = IoState_evalLoop_(state);
		IoCoroutine *active = IoState_currentCoroutine(state);

		IoCoroutine_saveState_(active, state);
		if (state->currentFrame) {
			// Left the loop mid-coroutine, which hostCanSwitch should have
			// prevented. Leave that coroutine parked and try again shortly
			// rather than trusting this tick's result.
			result = IoState_numberWithDouble_(state, 0.05);
			break;
		}
		if (active == sched || state->shouldExit) break;

		// A coroutine other than the scheduler finished, and nothing that
		// started it is still alive, so the eval loop returned here instead
		// of switching. Record how it ended and carry on with the scheduler.
		IoCoroutine_rawSetResult_(active, result);
		if (active != evalCoro && IoCoroutine_rawException(active) != state->ioNil) {
			IoCoroutine_rawPrintBackTrace(active);
		}
		state->errorRaised = 0;
		if (active == bg_stopping) bg_stopping = NULL;
		IoState_setCurrentCoroutine_(state, sched);
		release_finished(active);
		if (CORO_DATA(sched)->frameStack == NULL) {
			result = IoState_numberWithDouble_(state, 0.05);   // scheduler lost its place; as above
			break;
		}
		IoCoroutine_restoreState_(sched, state);
	}
	state->nestedEvalDepth--;
	state->errorRaised = 0;
	tick_sched = NULL;

	next_wake = ISNUMBER(result) ? IoNumber_asDouble(result) : -1;
	nothing_due = next_wake < -1.5;

	IoState_setCurrentCoroutine_(state, previous);
	IoCoroutine_restoreState_(previous, state);
	CORO_DATA(sched)->hasFinished = 1;
	release_finished(sched);
	// Callers only tick when no Io code is running, so the coroutine we
	// return to has no frames. Don't trust a frame pointer it saved earlier.
	state->currentFrame = NULL;
	state->frameDepth = 0;
}

// Run a message sent from JS (a proxy call, an event handler) as its own
// coroutine, so it can wait without blocking JS. Returns 1 with *result or
// *exception set if it finished; 0 if it parked, in which case later ticks
// carry it on and JS gets nil. Must not be called while Io code is running.
int io_browser_send(IoObject *target, IoMessage *msg, IoObject **result, IoObject **exception) {
	IoObject *scheduler = scheduler_object();
	IoCoroutine *coro = IoCoroutine_new(state);   // kept alive by io_send's retain pool
	IoCoroutine_rawSetRunTarget_(coro, target);
	IoCoroutine_rawSetRunLocals_(coro, target);
	IoCoroutine_rawSetRunMessage_(coro, msg);
	IoObject_setSlot_to_(scheduler, IoState_symbolWithCString_(state, "hostStart"), coro);

	// No time slices: JS is waiting for the result of this call
	run_scheduler(0);

	int finished = CORO_DATA(coro)->hasFinished;
	*exception = NULL;
	*result = state->ioNil;
	if (finished) {
		IoObject *e = IoCoroutine_rawException(coro);
		if (e != state->ioNil) *exception = e;
		else *result = IoCoroutine_rawResult(coro);
		release_finished(coro);   // result and exception are held by its slots
	}
	return finished;
}

// If the eval in progress has finished, write its result and return its
// status (0 or 1). Returns 2 while it is still running, 3 if there is none.
static int finish_eval(void) {
	if (!evalCoro) return 3;
	int finished = CORO_DATA(evalCoro)->hasFinished;
	if (!finished && !nothing_due) return 2;

	IoObject *coro = evalCoro;
	int status = 0;
	evalCoro = NULL;
	interrupt_requested = 0;

	if (!finished) {
		// Paused with no coroutine, timer or Promise due to wake it. Native Io
		// raises the same error. The coroutine stays parked: a later JS
		// callback can still resume it, as a background coroutine.
		static const char *msg =
			"\n  Exception: Scheduler: nothing left to resume\n"
			"  ---------\n"
			"  the eval is paused, and no coroutine, timer or Promise is due to wake it\n\n";
		output_append(msg, strlen(msg));
		status = 1;
	} else if (IoCoroutine_rawException(coro) != state->ioNil) {
		IoState_exception_(state, coro);   // prints the backtrace
		status = 1;
	} else {
		// With no printed output, show the result (already a string; see hostEval)
		IoObject *result = IoCoroutine_rawResult(coro);
		if (output_len == 0 && result && ISSEQ(result) && IoSeq_rawSize(result) > 0) {
			output_append("==> ", 4);
			output_append(CSTRING(result), IoSeq_rawSize(result));
			output_append("\n", 1);
		}
	}
	state->errorRaised = 0;
	eval_output_len = output_len;
	release_finished(coro);
	// Last: this slot is what kept the coroutine alive until here
	IoObject_setSlot_to_(scheduler_object(),
		IoState_symbolWithCString_(state, "hostEvalCoro"), state->ioNil);
	return status;
}

// Evaluate code when Io is already running (JS called back into eval from
// inside an Io call). Runs to completion on the caller's coroutine.
static int do_nested_eval(const char *code) {
	size_t before = output_len;
	exception_raised = 0;
	IoObject *result = IoState_doCString_(state, code);
	if (state->errorRaised || exception_raised) {
		state->errorRaised = 0;
		return 1;
	}
	if (output_len == before && result && result != state->ioNil) {
		IoSymbol *str = IoObject_asString_(result, NULL);
		if (str && !state->errorRaised && IoSeq_rawSize(str) > 0) {
			output_append("==> ", 4);
			output_append(CSTRING(str), IoSeq_rawSize(str));
			output_append("\n", 1);
		}
	}
	return 0;
}

// Internal eval: takes a C string, captures output, returns status.
// Returns: 0=success, 1=error, 2=still running (a later io_tick finishes it)
static int do_eval(const char *code) {
	if (!state) return -1;
	if (state->currentFrame) return do_nested_eval(code);
	if (evalCoro) return -1;   // the previous eval is still running

	// Everything allocated here, outside any coroutine, lands on the main
	// coroutine's retain stack. Without a pool around it nothing would ever
	// release those objects.
	IoState_pushRetainPool(state);

	output_clear();
	eval_output_len = 0;
	state->errorRaised = 0;
	exception_raised = 0;

	IoObject *scheduler = scheduler_object();
	IoMessage *m = IoMessage_newWithName_andCachedArg_(state,
		IoState_symbolWithCString_(state, "hostEval"),
		IoState_symbolWithCString_(state, code));

	// Scheduler's hostEvalCoro slot keeps it alive across ticks
	evalCoro = IoCoroutine_new(state);
	IoCoroutine_rawSetRunTarget_(evalCoro, scheduler);
	IoCoroutine_rawSetRunLocals_(evalCoro, scheduler);
	IoCoroutine_rawSetRunMessage_(evalCoro, m);
	IoObject_setSlot_to_(scheduler, IoState_symbolWithCString_(state, "hostEvalCoro"), evalCoro);
	IoObject_setSlot_to_(scheduler, IoState_symbolWithCString_(state, "hostStart"), evalCoro);

	run_scheduler(1);
	int status = finish_eval();
	IoState_popRetainPool(state);
	return status;
}

// Exported: evaluate code from the input buffer (preferred — no pointer issues).
// JS writes code into io_get_input_buf(), then calls this.
__attribute__((export_name("io_eval_input")))
int io_eval_input(void) {
	input_buf[INPUT_BUF_SIZE - 1] = '\0'; // safety
	return do_eval(input_buf);
}

// Exported: run runnable coroutines for one time slice. Returns the status
// of the eval in progress: 0 or 1 if it just finished, 2 if it is still
// running, 3 if there is none.
__attribute__((export_name("io_tick")))
int io_tick(void) {
	if (!state || state->currentFrame) return evalCoro ? 2 : 3;
	IoState_pushRetainPool(state);   // see do_eval
	run_scheduler(1);
	int status = finish_eval();
	IoState_popRetainPool(state);
	return status;
}

// Exported: milliseconds until io_tick should next be called. 0 = as soon
// as possible; negative = not until JS gives Io something to do (a Promise
// settles, or JS calls in).
__attribute__((export_name("io_next_wake_ms")))
double io_next_wake_ms(void) {
	return next_wake < 0 ? -1 : next_wake * 1000.0;
}

// Exported: how many bytes of the output buffer belong to the last eval.
// Anything after that was printed by other coroutines once it finished.
__attribute__((export_name("io_get_eval_output_len")))
size_t io_get_eval_output_len(void) {
	return eval_output_len;
}

// Exported: evaluate an Io expression by pointer (for advanced use).
__attribute__((export_name("io_eval")))
int io_eval(const char *code) {
	return do_eval(code);
}

// Exported: get pointer to output buffer (null-terminated).
__attribute__((export_name("io_get_output")))
const char *io_get_output(void) {
	if (!output_buf) return "";
	return output_buf;
}

// Exported: get length of output buffer.
__attribute__((export_name("io_get_output_len")))
size_t io_get_output_len(void) {
	return output_len;
}
