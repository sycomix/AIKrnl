#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int uart_send_file(const char* devpath, const void* buf, size_t len) {
    int fd = open(devpath, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) return -1;
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) { close(fd); return -1; }
    cfsetospeed(&tty, B115200); cfsetispeed(&tty, B115200);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tcsetattr(fd, TCSANOW, &tty);
    ssize_t w = write(fd, buf, len);
    close(fd);
    return (w == (ssize_t)len) ? 0 : -1;
}

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "Usage: %s <device>\n", argv[0]); return 2; }
    const char* dev = argv[1];
    const char* msg = "AIOS FPGA test\n";
    int rc = uart_send_file(dev, msg, strlen(msg));
    if (rc == 0) printf("sent to %s\n", dev); else printf("failed to send\n");
    return rc;
}
