#ifndef FPU_CONTEXT_H
#define FPU_CONTEXT_H

#include <fenv.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    fenv_t env;
} fpu_context_t;

void fpu_save(fpu_context_t* ctx);
void fpu_restore(const fpu_context_t* ctx);

#ifdef __cplusplus
}
#endif

#endif /* FPU_CONTEXT_H */
