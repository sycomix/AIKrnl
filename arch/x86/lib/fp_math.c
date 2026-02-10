#include "fp_math.h"
#include <stddef.h>
#include <stdio.h>

#if defined(__x86_64__)
#include <immintrin.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
static int has_avx512f(void) {
#ifdef __x86_64__
    return __builtin_cpu_supports("avx512f");
#else
    return 0;
#endif
}
#else
static int has_avx512f(void) { return 0; }
#endif

#ifdef USE_FPGA
#include "../../drivers/accel/fpga.h"
#endif

void fp_add_double(const double* a, const double* b, double* out, size_t n) {
    if (!a || !b || !out) { fprintf(stderr, "fp_add_double: NULL pointer\n"); return; }

#ifdef USE_FPGA
    if (fpga_add_double(a,b,out,n) == 0) return;
#endif

    size_t i = 0;

#if defined(__x86_64__)
    if (has_avx512f()) {
        for (; i + 8 <= n; i += 8) {
            __m512d va = _mm512_loadu_pd(a + i);
            __m512d vb = _mm512_loadu_pd(b + i);
            __m512d vr = _mm512_add_pd(va, vb);
            _mm512_storeu_pd(out + i, vr);
        }
    }
#endif
    for (; i < n; ++i) out[i] = a[i] + b[i];
}

void fp_mul_double(const double* a, const double* b, double* out, size_t n) {
    if (!a || !b || !out) { fprintf(stderr, "fp_mul_double: NULL pointer\n"); return; }

#ifdef USE_FPGA
    if (fpga_mul_double(a,b,out,n) == 0) return;
#endif

    size_t i = 0;

#if defined(__x86_64__)
    if (has_avx512f()) {
        for (; i + 8 <= n; i += 8) {
            __m512d va = _mm512_loadu_pd(a + i);
            __m512d vb = _mm512_loadu_pd(b + i);
            __m512d vr = _mm512_mul_pd(va, vb);
            _mm512_storeu_pd(out + i, vr);
        }
    }
#endif
    for (; i < n; ++i) out[i] = a[i] * b[i];
}
