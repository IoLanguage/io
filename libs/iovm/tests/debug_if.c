// Debug test for 'if' primitive

#include "IoState.h"
#include "IoObject.h"
#include "IoMessage.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoState_eval.h"
#include <stdio.h>

IoMessage *parseCode(IoState *state, const char *code) {
    IoSymbol *label = IoState_symbolWithCString_(state, "[debug]");
    return IoMessage_newFromText_labelSymbol_(state, (char *)code, label);
}

IoObject *evalCode(IoState *state, const char *code) {
    IoMessage *msg = parseCode(state, code);
    return IoMessage_locals_performOn_iterative(msg, state->lobby, state->lobby);
}

int main(int argc, char **argv) {
    printf("=== IF Primitive Debug Test ===\n");

    IoState *state = IoState_new();
    IoState_init(state);

    // Enable debug output
    state->showAllMessages = 1;

    printf("\n1. Setting counter to 0\n");
    evalCode(state, "counter := 0");

    printf("\n2. Defining increment method\n");
    evalCode(state, "increment := method(counter = counter + 1)");

    printf("\n3. Calling if(true, increment, increment)\n");
    evalCode(state, "if(true, increment, increment)");

    printf("\n4. Getting counter value\n");
    IoObject *result = evalCode(state, "counter");

    printf("\n=== Result ===\n");
    printf("counter = %d (expected 1)\n", ISNUMBER(result) ? IoNumber_asInt(result) : -999);

    return 0;
}
