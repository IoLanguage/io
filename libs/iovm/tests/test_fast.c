
// Quick test of fast evaluator

#include "IoState.h"
#include "IoObject.h"
#include "IoMessage.h"
#include "IoNumber.h"
#include "IoState_eval.h"
#include <stdio.h>

int main() {
    printf("Testing fast evaluator...\n");

    IoState *state = IoState_new();
    IoState_init(state);

    IoSymbol *label = IoState_symbolWithCString_(state, "[test]");
    IoMessage *msg = IoMessage_newFromText_labelSymbol_(state, "2 + 3", label);

    printf("Calling fast evaluator...\n");
    IoObject *result = IoMessage_locals_performOn_fast(msg, state->lobby, state->lobby);

    if (ISNUMBER(result)) {
        printf("Result: %d\n", IoNumber_asInt(result));
        printf("SUCCESS!\n");
        return 0;
    } else {
        printf("FAILED - not a number\n");
        return 1;
    }
}
