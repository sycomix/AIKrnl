#include "fpu_context.h"
#include <fenv.h>

void fpu_save(fpu_context_t* ctx) {
    if (!ctx) return;
    fegetenv(&ctx->env);
}

void fpu_restore(const fpu_context_t* ctx) {
    if (!ctx) return;
    fesetenv(&ctx->env);
}
