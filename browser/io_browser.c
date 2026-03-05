// Browser entry point for Io VM
// Compiled with -mexec-model=reactor to export functions for JS to call.
// Output is captured via printCallback into a growable buffer.

#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "UArray.h"
#include <stdlib.h>
#include <string.h>

static IoState *state = NULL;

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

// Exception callback: capture exception text
static void browser_exception_callback(void *context, IoObject *coroutine) {
	(void)context;
	// Use the default backtrace printing — it goes through printCallback
	IoCoroutine_rawPrintBackTrace(coroutine);
}

void IoAddonsInit(IoObject *context) { (void)context; }

// Exported: initialize the Io VM. Call once on startup.
__attribute__((export_name("io_init")))
int io_init(void) {
	if (state) return 0; // already initialized

	state = IoState_new();
	IoState_init(state);

	// Wire output capture
	IoState_printCallback_(state, browser_print_callback);
	IoState_exceptionCallback_(state, browser_exception_callback);

	// Set minimal args so System args doesn't crash
	const char *argv[] = {"io"};
	IoState_argc_argv_(state, 1, argv);

	output_clear();
	return 0;
}

// Internal eval: takes a C string, captures output, returns status.
static int do_eval(const char *code) {
	if (!state) return -1;

	output_clear();
	state->errorRaised = 0;

	IoObject *result = IoState_doCString_(state, code);

	if (state->errorRaised) {
		return 1;
	}

	// If there was no printed output but there is a result, show it
	if (output_len == 0 && result && result != state->ioNil) {
		IoSymbol *str = IoObject_asString_(result, NULL);
		if (str && !state->errorRaised) {
			const char *cstr = CSTRING(str);
			if (cstr && strlen(cstr) > 0) {
				output_append("==> ", 4);
				output_append(cstr, strlen(cstr));
				output_append("\n", 1);
			}
		}
	}

	return 0;
}

// Exported: evaluate code from the input buffer (preferred — no pointer issues).
// JS writes code into io_get_input_buf(), then calls this.
__attribute__((export_name("io_eval_input")))
int io_eval_input(void) {
	input_buf[INPUT_BUF_SIZE - 1] = '\0'; // safety
	return do_eval(input_buf);
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
