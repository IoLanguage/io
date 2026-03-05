
// Test file for iterative (non-recursive) evaluation

#include "IoState.h"
#include "IoObject.h"
#include "IoMessage.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoState_eval.h"
#include <stdio.h>
#include <string.h>

// Helper to create a test message from code
IoMessage *testParseCode(IoState *state, const char *code) {
    IoSymbol *label = IoState_symbolWithCString_(state, "[test]");
    return IoMessage_newFromText_labelSymbol_(state, (char *)code, label);
}

// Helper to evaluate code iteratively
IoObject *testEvalCode(IoState *state, const char *code) {
    IoMessage *msg = testParseCode(state, code);
    return IoMessage_locals_performOn_iterative(msg, state->lobby, state->lobby);
}

// Test simple literal evaluation
int test_literal(IoState *state) {
    printf("Test 1: Literal evaluation... ");

    IoObject *result = testEvalCode(state, "42");
    if (!ISNUMBER(result) || IoNumber_asInt(result) != 42) {
        printf("FAILED\n");
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test simple message send
int test_message_send(IoState *state) {
    printf("Test 2: Simple message send... ");

    IoObject *result = testEvalCode(state, "2 + 3");
    if (!ISNUMBER(result) || IoNumber_asInt(result) != 5) {
        printf("FAILED (got %d)\n", ISNUMBER(result) ? IoNumber_asInt(result) : -1);
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test message chain
int test_message_chain(IoState *state) {
    printf("Test 3: Message chain... ");

    // Simple chain without operator precedence complexity
    IoObject *result = testEvalCode(state, "2 + 3 + 4");
    if (!ISNUMBER(result) || IoNumber_asInt(result) != 9) {
        printf("FAILED (got %d, expected 9)\n",
               ISNUMBER(result) ? IoNumber_asInt(result) : -1);
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test slot assignment
int test_slot_assignment(IoState *state) {
    printf("Test 4: Slot assignment... ");

    testEvalCode(state, "testValue := 123");
    IoObject *result = testEvalCode(state, "testValue");

    if (!ISNUMBER(result) || IoNumber_asInt(result) != 123) {
        printf("FAILED\n");
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test block evaluation
int test_block_eval(IoState *state) {
    printf("Test 5: Block evaluation... ");

    IoObject *result = testEvalCode(state, "block(42) call");
    if (!ISNUMBER(result) || IoNumber_asInt(result) != 42) {
        printf("FAILED\n");
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test block with arguments
int test_block_args(IoState *state) {
    printf("Test 6: Block with arguments... ");

    testEvalCode(state, "addFunc := method(a, b, a + b)");
    IoObject *result = testEvalCode(state, "addFunc(10, 20)");

    if (!ISNUMBER(result) || IoNumber_asInt(result) != 30) {
        printf("FAILED (got %d)\n", ISNUMBER(result) ? IoNumber_asInt(result) : -1);
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test semicolon (statement separator)
int test_semicolon(IoState *state) {
    printf("Test 7: Semicolon statement separator... ");

    testEvalCode(state, "x := 5");
    IoObject *result = testEvalCode(state, "x := 10; x := 20; x");

    if (!ISNUMBER(result) || IoNumber_asInt(result) != 20) {
        printf("FAILED\n");
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test nested blocks
int test_nested_blocks(IoState *state) {
    printf("Test 8: Nested blocks... ");

    // Simpler nested block test
    const char *code = "outer := method(x, x + 5); outer(10)";
    IoObject *result = testEvalCode(state, code);

    if (!ISNUMBER(result) || IoNumber_asInt(result) != 15) {
        printf("FAILED (got %d, expected 15)\n",
               ISNUMBER(result) ? IoNumber_asInt(result) : -1);
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test lazy argument evaluation
int test_lazy_args(IoState *state) {
    printf("Test 9: Lazy argument evaluation... ");

    // Disable verbose debug for this test
    int oldShowAll = state->showAllMessages;
    state->showAllMessages = 0;

    // if() is a special form that doesn't evaluate all arguments
    printf("\n>>> Setting up counter and increment method\n");
    testEvalCode(state, "counter := 0");
    testEvalCode(state, "increment := method(counter = counter + 1)");

    printf(">>> Calling if(true, increment, increment)\n");
    state->showAllMessages = 1;  // Enable for just the if call
    testEvalCode(state, "if(true, increment, increment)");
    state->showAllMessages = 0;

    printf(">>> Getting counter value\n");
    IoObject *result = testEvalCode(state, "counter");

    // Restore debug setting
    state->showAllMessages = oldShowAll;

    // Should only increment once (true branch)
    if (!ISNUMBER(result) || IoNumber_asInt(result) != 1) {
        printf("FAILED (counter = %d, expected 1)\n",
               ISNUMBER(result) ? IoNumber_asInt(result) : -1);
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test list operations
int test_list(IoState *state) {
    printf("Test 10: List operations... ");

    testEvalCode(state, "myList := list(1, 2, 3)");
    IoObject *result = testEvalCode(state, "myList size");

    if (!ISNUMBER(result) || IoNumber_asInt(result) != 3) {
        printf("FAILED\n");
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

#ifdef IO_CALLCC
// Test callcc (call with current continuation)
int test_callcc_normal(IoState *state) {
    printf("Test 11: callcc normal return... ");

    // callcc with a block that returns normally
    // The result should be the block's return value
    IoObject *result = testEvalCode(state,
        "callcc(block(cont, \"normal return\"))");

    if (!ISSEQ(result)) {
        printf("FAILED (not a sequence)\n");
        return 0;
    }

    if (strcmp(CSTRING(result), "normal return") != 0) {
        printf("FAILED (got '%s', expected 'normal return')\n", CSTRING(result));
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test callcc with continuation invocation
int test_callcc_invoke(IoState *state) {
    printf("Test 12: callcc continuation invoke... ");

    // callcc with a block that invokes the continuation
    // The result should be the value passed to invoke
    IoObject *result = testEvalCode(state,
        "callcc(block(cont, cont invoke(42); 999))");

    if (!ISNUMBER(result)) {
        printf("FAILED (not a number, got type %s)\n",
               IoObject_name(result));
        return 0;
    }

    if (IoNumber_asInt(result) != 42) {
        printf("FAILED (got %d, expected 42)\n", IoNumber_asInt(result));
        return 0;
    }

    printf("PASSED\n");
    return 1;
}

// Test continuation as escape mechanism
int test_callcc_escape(IoState *state) {
    printf("Test 13: callcc escape pattern... ");

    // Use continuation to escape from nested evaluation
    // The "normal" value should never be reached
    IoObject *result = testEvalCode(state,
        "callcc(block(escape, if(true, escape invoke(\"escaped\")); \"normal\"))");

    if (!ISSEQ(result)) {
        printf("FAILED (not a sequence, got type %s)\n",
               IoObject_name(result));
        return 0;
    }

    if (strcmp(CSTRING(result), "escaped") != 0) {
        printf("FAILED (got '%s', expected 'escaped')\n", CSTRING(result));
        return 0;
    }

    printf("PASSED\n");
    return 1;
}
#endif /* IO_CALLCC */

// Test try/catch with Exception raise
int test_try_catch(IoState *state) {
	printf("Test 14: try/catch Exception raise... ");

	IoObject *result = testEvalCode(state,
		"e := try(Exception raise(\"test\")); e error");

	if (!ISSEQ(result)) {
		printf("FAILED (not a sequence, got type %s)\n",
			   IoObject_name(result));
		return 0;
	}

	if (strcmp(CSTRING(result), "test") != 0) {
		printf("FAILED (got '%s', expected 'test')\n", CSTRING(result));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test try with no exception
int test_try_no_exception(IoState *state) {
	printf("Test 15: try with no exception... ");

	IoObject *result = testEvalCode(state, "try(1 + 1)");

	if (result != state->ioNil) {
		printf("FAILED (expected nil, got type %s)\n",
			   IoObject_name(result));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test that execution continues after try captures exception
int test_try_continues(IoState *state) {
	printf("Test 16: execution continues after try... ");

	IoObject *result = testEvalCode(state,
		"e := try(Exception raise(\"boom\")); \"continued\"");

	if (!ISSEQ(result)) {
		printf("FAILED (not a sequence, got type %s)\n",
			   IoObject_name(result));
		return 0;
	}

	if (strcmp(CSTRING(result), "continued") != 0) {
		printf("FAILED (got '%s', expected 'continued')\n", CSTRING(result));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test exception pass (re-raise)
int test_exception_pass(IoState *state) {
	printf("Test 17: exception pass... ");

	IoObject *result = testEvalCode(state,
		"e := try(try(Exception raise(\"inner\")) pass); e error");

	if (!ISSEQ(result)) {
		printf("FAILED (not a sequence, got type %s)\n",
			   IoObject_name(result));
		return 0;
	}

	if (strcmp(CSTRING(result), "inner") != 0) {
		printf("FAILED (got '%s', expected 'inner')\n", CSTRING(result));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test uncaught C-level exception (unknown method)
int test_uncaught_exception(IoState *state) {
	printf("Test 18: uncaught exception (unknown method)... ");

	testEvalCode(state, "e := try(1 unknownMethod)");
	IoObject *result = testEvalCode(state, "e error");

	if (!ISSEQ(result)) {
		printf("FAILED (not a sequence, got type %s)\n",
			   IoObject_name(result));
		return 0;
	}

	if (strstr(CSTRING(result), "does not respond to") == NULL) {
		printf("FAILED (got '%s', expected to contain 'does not respond to')\n",
			   CSTRING(result));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test basic coroutine resume
int test_coro_resume_basic(IoState *state) {
	printf("Test 19: basic coroutine resume... ");

	// Create a coro, set it up to run code, resume it
	// After the coro finishes, control should return to main
	// Note: both runTarget and runLocals must be Lobby so that
	// updateSlot (=) can find the coroResult slot defined on Lobby
	IoObject *result = testEvalCode(state,
		"coroResult := nil; "
		"c := Coroutine clone; "
		"c setRunTarget(Lobby); "
		"c setRunLocals(Lobby); "
		"c setRunMessage(message(coroResult = \"from coro\")); "
		"c resume; "
		"coroResult");

	if (!ISSEQ(result)) {
		printf("FAILED (not a sequence, got type %s)\n",
			   IoObject_name(result));
		return 0;
	}

	if (strcmp(CSTRING(result), "from coro") != 0) {
		printf("FAILED (got '%s', expected 'from coro')\n", CSTRING(result));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test yield to queued coroutine
int test_coro_yield(IoState *state) {
	printf("Test 20: yield to queued coroutine... ");

	// yield with no queued coros should return nil and continue
	IoObject *result = testEvalCode(state, "yield; \"after yield\"");

	if (!ISSEQ(result)) {
		printf("FAILED (not a sequence, got type %s)\n",
			   IoObject_name(result));
		return 0;
	}

	if (strcmp(CSTRING(result), "after yield") != 0) {
		printf("FAILED (got '%s', expected 'after yield')\n", CSTRING(result));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test @@ async dispatch (actor pattern)
int test_coro_async(IoState *state) {
	printf("Test 21: @@ async dispatch... ");

	// @@ creates a future and runs in an actor coroutine
	// Use Lobby setSlot since the method's locals don't inherit from Lobby
	IoObject *result = testEvalCode(state,
		"asyncResult := nil; "
		"o := Object clone; "
		"o test := method(Lobby setSlot(\"asyncResult\", \"async done\")); "
		"o @@test; yield; "
		"asyncResult");

	if (!ISSEQ(result)) {
		printf("FAILED (not a sequence, got type %s)\n",
			   IoObject_name(result));
		return 0;
	}

	if (strcmp(CSTRING(result), "async done") != 0) {
		printf("FAILED (got '%s', expected 'async done')\n", CSTRING(result));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test tail call optimization (direct tail recursion)
int test_tco_direct(IoState *state) {
	printf("Test 22: TCO direct tail recursion... ");

	// Direct tail recursion: last expression in method body is a self-call
	// Without TCO this would use 10000+ frames; with TCO it stays flat
	testEvalCode(state,
		"countdown := method(n, if(n <= 0, return n); countdown(n - 1))");
	IoObject *result = testEvalCode(state, "countdown(10000)");

	if (!ISNUMBER(result) || IoNumber_asInt(result) != 0) {
		printf("FAILED (got %d, expected 0)\n",
			   ISNUMBER(result) ? IoNumber_asInt(result) : -1);
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test deep recursion through if (heap frames, no C stack overflow)
int test_deep_recursion(IoState *state) {
	printf("Test 23: deep recursion through if... ");

	// This recursion goes through if branches, so TCO may not apply,
	// but heap-allocated frames prevent C stack overflow
	testEvalCode(state,
		"sumTo := method(n, if(n <= 0, 0, n + sumTo(n - 1)))");
	IoObject *result = testEvalCode(state, "sumTo(100)");

	// sum of 1..100 = 5050
	if (!ISNUMBER(result) || IoNumber_asInt(result) != 5050) {
		printf("FAILED (got %d, expected 5050)\n",
			   ISNUMBER(result) ? IoNumber_asInt(result) : -1);
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test tail-recursive accumulator pattern
int test_tco_accumulator(IoState *state) {
	printf("Test 24: TCO accumulator pattern... ");

	// Tail-recursive sum with accumulator
	// The recursive call is the last expression in the method
	testEvalCode(state,
		"sumAcc := method(n, acc, if(n <= 0, return acc); sumAcc(n - 1, acc + n))");
	IoObject *result = testEvalCode(state, "sumAcc(10000, 0)");

	// sum of 1..10000 = 50005000
	if (!ISNUMBER(result)) {
		printf("FAILED (not a number)\n");
		return 0;
	}

	double expected = 50005000.0;
	double got = IoNumber_asDouble(result);
	if (got != expected) {
		printf("FAILED (got %.0f, expected %.0f)\n", got, expected);
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test TCO through if branches (the most important TCO pattern)
int test_tco_through_if(IoState *state) {
	printf("Test 25: TCO through if branches... ");

	// Classic tail-recursive pattern: the recursive call is inside
	// an if branch, which is the last expression in the method.
	// This tests the tail position optimization in IF_EVAL_BRANCH.
	testEvalCode(state,
		"factorial := method(n, acc, if(n <= 1, acc, factorial(n - 1, n * acc)))");
	IoObject *result = testEvalCode(state, "factorial(20, 1)");

	if (!ISNUMBER(result)) {
		printf("FAILED (not a number)\n");
		return 0;
	}

	// 20! = 2432902008176640000 (fits in double)
	double expected = 2432902008176640000.0;
	double got = IoNumber_asDouble(result);
	if (got != expected) {
		printf("FAILED (got %.0f, expected %.0f)\n", got, expected);
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test TCO through if with large recursion depth
int test_tco_if_deep(IoState *state) {
	printf("Test 26: TCO through if (deep recursion)... ");

	// Without TCO through if, this would use 100000+ frames.
	// With TCO through if, the frame stack stays bounded.
	testEvalCode(state,
		"countDown := method(n, acc, if(n <= 0, acc, countDown(n - 1, acc + 1)))");
	IoObject *result = testEvalCode(state, "countDown(100000, 0)");

	if (!ISNUMBER(result)) {
		printf("FAILED (not a number)\n");
		return 0;
	}

	double expected = 100000.0;
	double got = IoNumber_asDouble(result);
	if (got != expected) {
		printf("FAILED (got %.0f, expected %.0f)\n", got, expected);
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

#ifdef IO_CALLCC
// Test continuation introspection methods
int test_continuation_introspection(IoState *state) {
	printf("Test 27: Continuation introspection... ");

	// Capture a continuation and inspect it.
	// Use copy to snapshot the frame chain while it's live —
	// with grab-pointer capture, the original frame evolves after callcc returns.
	testEvalCode(state,
		"captured := nil\n"
		"callcc(block(cont, captured = cont copy))");

	IoObject *frameCount = testEvalCode(state, "captured frameCount");
	if (!ISNUMBER(frameCount) || IoNumber_asInt(frameCount) < 1) {
		printf("FAILED (frameCount=%d, expected >= 1)\n",
			ISNUMBER(frameCount) ? (int)IoNumber_asInt(frameCount) : -1);
		return 0;
	}

	IoObject *states = testEvalCode(state, "captured frameStates");
	if (!ISLIST(states)) {
		printf("FAILED (frameStates not a list)\n");
		return 0;
	}

	IoObject *messages = testEvalCode(state, "captured frameMessages");
	if (!ISLIST(messages)) {
		printf("FAILED (frameMessages not a list)\n");
		return 0;
	}

	// First frame state should be callcc:evalBlock
	IoObject *firstState = testEvalCode(state, "captured frameStates first");
	if (!ISSEQ(firstState)) {
		printf("FAILED (first state not a string)\n");
		return 0;
	}
	const char *stateName = CSTRING(firstState);
	if (strcmp(stateName, "callcc:evalBlock") != 0) {
		printf("FAILED (first state='%s', expected 'callcc:evalBlock')\n", stateName);
		return 0;
	}

	printf("PASSED\n");
	return 1;
}
#endif /* IO_CALLCC */

// Test TCO through if with relayStopStatus (? operator)
int test_tco_if_stop_status(IoState *state) {
	printf("Test 28: TCO through if + stop status (? operator)... ");

	// This tests the savedCall mechanism: the ? operator uses
	// relayStopStatus which sets Call stop status. With the in-place
	// if optimization, TCO could replace frame->call and lose the
	// stop status. The savedCall field preserves it.
	//
	// Pattern from ObjectTest: x ?return "first" sends return to x (nil),
	// which triggers RETURN stop status. The ? method must propagate
	// this via relayStopStatus.
	testEvalCode(state,
		"a := method(x, x return \"first\"; \"second\")\n"
		"b := method(x, x ?return \"first\"; \"second\")");

	IoObject *resultA = testEvalCode(state, "a");
	IoObject *resultB = testEvalCode(state, "b");

	if (!ISSEQ(resultA)) {
		printf("FAILED (a not a string, got %s)\n", IoObject_name(resultA));
		return 0;
	}
	if (strcmp(CSTRING(resultA), "first") != 0) {
		printf("FAILED (a='%s', expected 'first')\n", CSTRING(resultA));
		return 0;
	}
	if (!ISSEQ(resultB)) {
		printf("FAILED (b not a string, got %s)\n", IoObject_name(resultB));
		return 0;
	}
	if (strcmp(CSTRING(resultA), CSTRING(resultB)) != 0) {
		printf("FAILED (a='%s' != b='%s')\n", CSTRING(resultA), CSTRING(resultB));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

// Test ? operator with continue in foreach (stop status propagation)
int test_question_mark_continue(IoState *state) {
	printf("Test 29: ? operator with continue (stop status)... ");

	// From ObjectTest: ?continue in foreach should behave like continue
	testEvalCode(state,
		"a := method(\n"
		"    r := list\n"
		"    list(1,2,3,4,5) foreach(x,\n"
		"        if(x > 3, continue)\n"
		"        r append(x)\n"
		"    )\n"
		"    r\n"
		")\n"
		"b := method(\n"
		"    r := list\n"
		"    list(1,2,3,4,5) foreach(x,\n"
		"        if(x > 3, ?continue)\n"
		"        r append(x)\n"
		"    )\n"
		"    r\n"
		")");

	IoObject *resultA = testEvalCode(state, "a");
	IoObject *resultB = testEvalCode(state, "b");

	if (!ISLIST(resultA) || !ISLIST(resultB)) {
		printf("FAILED (not lists)\n");
		return 0;
	}

	// Both should be list(1, 2, 3) - items <= 3
	IoObject *sizeA = testEvalCode(state, "a size");
	IoObject *sizeB = testEvalCode(state, "b size");
	if (IoNumber_asInt(sizeA) != 3 || IoNumber_asInt(sizeB) != 3) {
		printf("FAILED (sizes: a=%d, b=%d, expected 3)\n",
			(int)IoNumber_asInt(sizeA), (int)IoNumber_asInt(sizeB));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}

#ifdef IO_CALLCC
// Test Continuation asMap serialization
int test_continuation_asMap(IoState *state) {
	printf("Test 30: Continuation asMap... ");

	// Capture a continuation inside a for loop.
	// Use copy to snapshot while live (grab-pointer frames evolve after return).
	testEvalCode(state,
		"captured2 := nil\n"
		"for(i, 1, 5, if(i == 3, callcc(block(cont, captured2 = cont copy))))");


	// Check asMap returns a Map
	IoObject *map = testEvalCode(state, "captured2 asMap");
	if (!IoObject_slots(map)) {
		printf("FAILED (asMap didn't return a Map)\n");
		return 0;
	}

	// Check frameCount
	IoObject *fc = testEvalCode(state, "captured2 asMap at(\"frameCount\")");
	if (!ISNUMBER(fc) || IoNumber_asInt(fc) < 1) {
		printf("FAILED (frameCount < 1)\n");
		return 0;
	}

	// Check first frame is callcc:evalBlock
	IoObject *firstState = testEvalCode(state,
		"captured2 asMap at(\"frames\") first at(\"state\")");
	if (!ISSEQ(firstState) ||
		strcmp(CSTRING(firstState), "callcc:evalBlock") != 0) {
		printf("FAILED (first frame state != callcc:evalBlock)\n");
		return 0;
	}

	// Check for loop state was captured (currentValue should be 3)
	IoObject *forValue = testEvalCode(state,
		"captured2 asMap at(\"frames\") detect(at(\"state\") beginsWithSeq(\"for\")) at(\"currentValue\")");
	if (!ISNUMBER(forValue) || IoNumber_asDouble(forValue) != 3.0) {
		printf("FAILED (for currentValue != 3, got %s)\n",
			ISNUMBER(forValue) ? "wrong number" : IoObject_name(forValue));
		return 0;
	}

	printf("PASSED\n");
	return 1;
}
#endif /* IO_CALLCC */

int main(int argc, char **argv) {
    printf("=== Iterative Evaluator Tests ===\n\n");

    // Initialize Io state
    IoState *state = IoState_new();
    IoState_init(state);

    int passed = 0;
    int total = 0;

    // Enable debug output for lazy args test
    int enableDebug = (argc > 1 && strcmp(argv[1], "-debug") == 0);

    // Run tests
    total++; passed += test_literal(state);
    total++; passed += test_message_send(state);
    total++; passed += test_message_chain(state);
    total++; passed += test_slot_assignment(state);
    total++; passed += test_block_eval(state);
    total++; passed += test_block_args(state);
    total++; passed += test_semicolon(state);
    total++; passed += test_nested_blocks(state);
    total++; passed += test_lazy_args(state);
    total++; passed += test_list(state);
#ifdef IO_CALLCC
    total++; passed += test_callcc_normal(state);
    total++; passed += test_callcc_invoke(state);
    total++; passed += test_callcc_escape(state);
#endif
    total++; passed += test_try_catch(state);
    total++; passed += test_try_no_exception(state);
    total++; passed += test_try_continues(state);
    total++; passed += test_exception_pass(state);
    total++; passed += test_uncaught_exception(state);
    total++; passed += test_coro_resume_basic(state);
    total++; passed += test_coro_yield(state);
    total++; passed += test_coro_async(state);
    total++; passed += test_tco_direct(state);
    total++; passed += test_deep_recursion(state);
    total++; passed += test_tco_accumulator(state);
    total++; passed += test_tco_through_if(state);
    total++; passed += test_tco_if_deep(state);
#ifdef IO_CALLCC
    total++; passed += test_continuation_introspection(state);
#endif
    total++; passed += test_tco_if_stop_status(state);
    total++; passed += test_question_mark_continue(state);
#ifdef IO_CALLCC
    total++; passed += test_continuation_asMap(state);
#endif

    // Print summary
    printf("\n=== Results ===\n");
    printf("Passed: %d/%d\n", passed, total);

    if (passed == total) {
        printf("\nAll tests PASSED!\n");
        return 0;
    } else {
        printf("\nSome tests FAILED.\n");
        return 1;
    }
}
