#ifndef FP_MATH_H
#define FP_MATH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Element-wise addition: out[i] = a[i] + b[i] for i in [0, n).
 * Works with AVX-512 when available and falls back to scalar loop.
 */
void fp_add_double(const double* a, const double* b, double* out, size_t n);

/**
 * Element-wise multiplication: out[i] = a[i] * b[i] for i in [0, n).
 */
void fp_mul_double(const double* a, const double* b, double* out, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* FP_MATH_H */
