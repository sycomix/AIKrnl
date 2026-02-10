#include "../include/criu.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

static int have_criu(void) {
    return system("command -v criu >/dev/null 2>&1") == 0;
}

int criu_dump_pid(pid_t pid, const char* dir) {
    if (!have_criu()) return 77; // skip
    if (!dir) return -1;
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s && criu dump -t %d -D %s --shell-job --leave-running", dir, (int)pid, dir);
    int rc = system(cmd);
    return rc == 0 ? 0 : -1;
}

int criu_restore_dir(const char* dir, pid_t* out_pid) {
    if (!have_criu()) return 77; // skip
    if (!dir) return -1;
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "criu restore -D %s --shell-job -o %s/restore.log", dir, dir);
    int rc = system(cmd);
    if (rc != 0) return -1;
    if (out_pid) *out_pid = -1; // restored PID is managed by the system; retrieving it is complex
    return 0;
}
