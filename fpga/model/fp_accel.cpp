#include <cstddef>
#include <cstring>

extern "C" int fpga_model_add_double(const double* a, const double* b, double* out, size_t n) {
    if (!a || !b || !out) return -1;
    for (size_t i = 0; i < n; ++i) out[i] = a[i] + b[i];
    return 0;
}

extern "C" int fpga_model_mul_double(const double* a, const double* b, double* out, size_t n) {
    if (!a || !b || !out) return -1;
    for (size_t i = 0; i < n; ++i) out[i] = a[i] * b[i];
    return 0;
}
