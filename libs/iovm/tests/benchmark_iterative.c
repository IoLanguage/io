
// Benchmark comparing recursive vs iterative evaluators

#include "IoState.h"
#include "IoObject.h"
#include "IoMessage.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoState_eval.h"
#include <stdio.h>
#include <time.h>

#define ITERATIONS 100000

// Helper to parse code
IoMessage *parseCode(IoState *state, const char *code) {
    IoSymbol *label = IoState_symbolWithCString_(state, "[benchmark]");
    return IoMessage_newFromText_labelSymbol_(state, (char *)code, label);
}

// Benchmark recursive evaluator
double benchmarkRecursive(IoState *state, const char *code, int iterations) {
    IoMessage *msg = parseCode(state, code);
    clock_t start = clock();

    for (int i = 0; i < iterations; i++) {
        IoMessage_locals_performOn_(msg, state->lobby, state->lobby);
    }

    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

// Benchmark iterative evaluator
double benchmarkIterative(IoState *state, const char *code, int iterations) {
    IoMessage *msg = parseCode(state, code);
    clock_t start = clock();

    for (int i = 0; i < iterations; i++) {
        IoMessage_locals_performOn_iterative(msg, state->lobby, state->lobby);
    }

    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

void runBenchmark(IoState *state, const char *name, const char *code, int iterations) {
    printf("\n%s:\n", name);
    printf("  Code: %s\n", code);

    // Warm up
    parseCode(state, code);

    // Benchmark recursive
    double recursiveTime = benchmarkRecursive(state, code, iterations);
    double recursiveOpsPerSec = iterations / recursiveTime;

    // Benchmark iterative (now with frame pooling!)
    double iterativeTime = benchmarkIterative(state, code, iterations);
    double iterativeOpsPerSec = iterations / iterativeTime;

    printf("  Recursive: %.3f sec (%.2f M ops/sec)\n",
           recursiveTime, recursiveOpsPerSec / 1000000.0);
    printf("  Pooled:    %.3f sec (%.2f M ops/sec) - %.2fx overhead\n",
           iterativeTime, iterativeOpsPerSec / 1000000.0, iterativeTime / recursiveTime);

    double speedup = recursiveOpsPerSec / iterativeOpsPerSec;
    printf("  Target:    Need %.2fx faster to match Ruby goal\n", speedup * 10.0);
}

int main(int argc, char **argv) {
    printf("=== Io Evaluator Performance Benchmark ===\n");
    printf("Iterations: %d\n", ITERATIONS);

    IoState *state = IoState_new();
    IoState_init(state);

    // Set up some test data
    IoMessage *msg = parseCode(state, "x := 42; y := 100");
    IoMessage_locals_performOn_(msg, state->lobby, state->lobby);

    msg = parseCode(state, "add := method(a, b, a + b)");
    IoMessage_locals_performOn_(msg, state->lobby, state->lobby);

    // Run benchmarks
    runBenchmark(state, "Local Access", "x", ITERATIONS);
    runBenchmark(state, "Local Set", "x = 123", ITERATIONS);
    runBenchmark(state, "Slot Access", "List", ITERATIONS);
    runBenchmark(state, "Simple Message", "2 + 3", ITERATIONS);
    runBenchmark(state, "Message Chain", "2 + 3 + 4", ITERATIONS);
    runBenchmark(state, "Block Activation", "block(42) call", ITERATIONS);
    runBenchmark(state, "Method Call", "add(10, 20)", ITERATIONS / 10);

    printf("\n=== Summary ===\n");
    printf("The iterative evaluator trades some performance for:\n");
    printf("  - No C stack dependence (enables continuations)\n");
    printf("  - Serializable execution state\n");
    printf("  - Better stack overflow handling\n");
    printf("  - Network-portable coroutines\n");

    return 0;
}
