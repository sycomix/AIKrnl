#include <stdio.h>
#include <fenv.h>
#include "../src/scheduler/fpu_context.h"

int main(void) {
    fpu_context_t ctx;

    // set rounding to downward and save
    if (fesetround(FE_DOWNWARD) != 0) { printf("failed to set FE_DOWNWARD\n"); return 1; }
    if (fegetround() != FE_DOWNWARD) { printf("rounding not FE_DOWNWARD initially\n"); return 1; }

    fpu_save(&ctx);

    // change rounding to upward
    if (fesetround(FE_UPWARD) != 0) { printf("failed to set FE_UPWARD\n"); return 1; }
    if (fegetround() != FE_UPWARD) { printf("rounding not FE_UPWARD after change\n"); return 1; }

    // restore and validate
    fpu_restore(&ctx);
    if (fegetround() != FE_DOWNWARD) { printf("restore failed: expected FE_DOWNWARD got %d\n", fegetround()); return 1; }

    printf("FPU context save/restore passed\n");
    return 0;
}
