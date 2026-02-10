#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../drivers/accel/fpga.h"

static int double_close(double a, double b) {
    double diff = fabs(a - b);
    double tol = 1e-12;
    return diff <= tol || diff <= tol * fmax(fabs(a), fabs(b));
}

int main(void) {
    size_t n = 128;
    double* a = (double*)malloc(sizeof(double)*n);
    double* b = (double*)malloc(sizeof(double)*n);
    double* out = (double*)malloc(sizeof(double)*n);
    double* exp = (double*)malloc(sizeof(double)*n);
    for (size_t i = 0; i < n; ++i) { a[i]= (double)i; b[i]=(double)(2*i); exp[i]=a[i]+b[i]; }
    if (fpga_add_double(a,b,out,n) != 0) { printf("fpga add failed\n"); return 1; }
    for (size_t i = 0; i < n; ++i) if (!double_close(out[i], exp[i])) { printf("mismatch at %zu: %g vs %g\n", i, out[i], exp[i]); return 1; }
    for (size_t i = 0; i < n; ++i) exp[i]=a[i]*b[i];
    if (fpga_mul_double(a,b,out,n) != 0) { printf("fpga mul failed\n"); return 1; }
    for (size_t i = 0; i < n; ++i) if (!double_close(out[i], exp[i])) { printf("mismatch mul at %zu: %g vs %g\n", i, out[i], exp[i]); return 1; }
    free(a); free(b); free(out); free(exp);
    printf("FPGA HLS emulation tests passed\n");
    return 0;
}
