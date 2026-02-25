
// Test file for iterative (non-recursive) evaluation

#include "IoState.h"
#include "IoObject.h"
#include "IoMessage.h"
#include "IoSeq.h"
#include "IoNumber.h"
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
    total++; passed += test_callcc_normal(state);
    total++; passed += test_callcc_invoke(state);
    total++; passed += test_callcc_escape(state);

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
