#include "fpga.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Use the behavioral model implemented in C++ */
#ifdef __cplusplus
extern "C" {
#endif
int fpga_model_add_double(const double* a, const double* b, double* out, size_t n);
int fpga_model_mul_double(const double* a, const double* b, double* out, size_t n);
#ifdef __cplusplus
}
#endif

int fpga_add_double(const double* a, const double* b, double* out, size_t n) {
    if (!a || !b || !out) { fprintf(stderr, "fpga_add_double: NULL pointer\n"); return -1; }
    return fpga_model_add_double(a,b,out,n);
}

int fpga_mul_double(const double* a, const double* b, double* out, size_t n) {
    if (!a || !b || !out) { fprintf(stderr, "fpga_mul_double: NULL pointer\n"); return -1; }
    return fpga_model_mul_double(a,b,out,n);
}

int fpga_uart_send(const void* data, size_t size, const char* devpath) {
    (void)data; (void)size; (void)devpath;
    /* PoC: in emulation mode we don't have actual UART, so just return success */
    return 0;
}
