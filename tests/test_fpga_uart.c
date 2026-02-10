#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void) {
    const char* env = getenv("AIOS_FPGA_UART");
    const char* dev = env ? env : "/dev/ttyUSB0";
    struct stat st;
    if (stat(dev, &st) != 0) {
        fprintf(stderr, "UART device %s not found; skipping test\n", dev);
        return 77; // skip
    }
    // If device exists, ensure we can open it (no write expected in CI)
    FILE* f = fopen(dev, "w");
    if (!f) { fprintf(stderr, "Cannot open %s for writing\n", dev); return 1; }
    fprintf(f, "aios_uart_test\n"); fclose(f);
    printf("UART test wrote to %s\n", dev);
    return 0;
}
