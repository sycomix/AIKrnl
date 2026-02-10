#ifndef FPGA_H
#define FPGA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Perform element-wise addition on n doubles: out[i]=a[i]+b[i]. Returns 0 on success. */
int fpga_add_double(const double* a, const double* b, double* out, size_t n);

/** Perform element-wise multiplication on n doubles: out[i]=a[i]*b[i]. Returns 0 on success. */
int fpga_mul_double(const double* a, const double* b, double* out, size_t n);

/* Host-side UART helper: send raw bytes to FPGA (blocking). Returns 0 on success. */
int fpga_uart_send(const void* data, size_t size, const char* devpath);

#ifdef __cplusplus
}
#endif

#endif /* FPGA_H */