#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../include/fp_math.h"

static int double_close(double a, double b) {
    double diff = fabs(a - b);
    double tol = 1e-12;
    return diff <= tol || diff <= tol * fmax(fabs(a), fabs(b));
}

static void fill_random(double* a, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] = (double)rand() / (double)RAND_MAX * 2.0 - 1.0; // [-1,1]
    }
}

static int test_add_fixed(void) {
    double a[8] = {0,1,2,3,4,5,6,7};
    double b[8] = {7,6,5,4,3,2,1,0};
    double out[8];
    double exp[8];
    for (size_t i = 0; i < 8; ++i) exp[i] = a[i] + b[i];
    fp_add_double(a,b,out,8);
    for (size_t i = 0; i < 8; ++i) {
        if (!double_close(out[i], exp[i])) {
            printf("test_add_fixed: mismatch at %zu: got %g expected %g\n", i, out[i], exp[i]);
            return 1;
        }
    }
    return 0;
}

static int test_mul_fixed(void) {
    double a[9];
    double b[9];
    double out[9];
    double exp[9];
    for (size_t i = 0; i < 9; ++i) { a[i] = i * 0.5; b[i] = (9 - i) * 0.25; exp[i] = a[i] * b[i]; }
    fp_mul_double(a,b,out,9);
    for (size_t i = 0; i < 9; ++i) {
        if (!double_close(out[i], exp[i])) {
            printf("test_mul_fixed: mismatch at %zu: got %g expected %g\n", i, out[i], exp[i]);
            return 1;
        }
    }
    return 0;
}

static int test_randomized(void) {
    const int trials = 200;
    for (int t = 0; t < trials; ++t) {
        size_t n = (rand() % 1000);
        double* a = malloc(sizeof(double) * n);
        double* b = malloc(sizeof(double) * n);
        double* out = malloc(sizeof(double) * n);
        double* exp = malloc(sizeof(double) * n);
        fill_random(a,n); fill_random(b,n);
        for (size_t i = 0; i < n; ++i) exp[i] = a[i] + b[i];
        fp_add_double(a,b,out,n);
        for (size_t i = 0; i < n; ++i) {
            if (!double_close(out[i], exp[i])) {
                printf("test_randomized add: t=%d n=%zu i=%zu got=%g expected=%g\n", t, n, i, out[i], exp[i]);
                return 1;
            }
        }
        for (size_t i = 0; i < n; ++i) exp[i] = a[i] * b[i];
        fp_mul_double(a,b,out,n);
        for (size_t i = 0; i < n; ++i) {
            if (!double_close(out[i], exp[i])) {
                printf("test_randomized mul: t=%d n=%zu i=%zu got=%g expected=%g\n", t, n, i, out[i], exp[i]);
                return 1;
            }
        }
        free(a); free(b); free(out); free(exp);
    }
    return 0;
}

int main(void) {
    srand(0);
    printf("FP math tests start\n");
    if (test_add_fixed()) return 1;
    if (test_mul_fixed()) return 1;
    if (test_randomized()) return 1;
    if (fp_add_double(NULL,NULL,NULL,0), 0) { /* ensure no-op on NULL; function prints error but shouldn't crash */ }
    printf("All FP math tests passed\n");
    return 0;
}
