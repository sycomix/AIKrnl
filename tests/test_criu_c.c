#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "../include/criu.h"

int main(void) {
    // Quick availability check
    if (system("command -v criu >/dev/null 2>&1") != 0) {
        fprintf(stderr, "CRIU not available; skipping test\n");
        return 77; // skip code
    }

    // Spawn a small worker that writes a file periodically
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        // child
        for (int i = 0; i < 60; ++i) {
            FILE* f = fopen("/tmp/aios_criu_worker_state.txt","w");
            if (f) { fprintf(f, "%d\n", i); fclose(f); }
            sleep(1);
        }
        return 0;
    }

    // Parent: attempt dump
    char tmpdir[] = "/tmp/aios-criu-XXXXXX";
    if (!mkdtemp(tmpdir)) {
        perror("mkdtemp"); kill(pid, SIGKILL); return 1;
    }

    int rc = criu_dump_pid(pid, tmpdir);
    if (rc == 77) {
        fprintf(stderr, "CRIU not available during dump; skipping test\n"); kill(pid, SIGKILL); return 77;
    }
    if (rc != 0) { fprintf(stderr, "criu_dump_pid failed\n"); kill(pid, SIGKILL); return 1; }

    // For PoC, we won't fully rely on restore being successful in all CI; attempt restore
    pid_t restored = -1;
    rc = criu_restore_dir(tmpdir, &restored);
    if (rc == 77) { fprintf(stderr, "CRIU not available during restore; skipping test\n"); kill(pid, SIGKILL); return 77; }
    if (rc != 0) { fprintf(stderr, "criu_restore_dir failed (this may need privileged runner)\n"); kill(pid, SIGKILL); return 1; }

    // Clean up
    kill(pid, SIGKILL);
    fprintf(stderr, "CRIU dump and restore attempted (PoC).\n");
    return 0;
}
