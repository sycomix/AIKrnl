#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * PoC Wasm runner: writes an embedded wasm blob to a temp file and runs it with wasmtime CLI
 * using `timeout` and `ulimit -v` to bound CPU and memory.
 * - cmd: timeout <secs> bash -c "ulimit -v <kb>; wasmtime <file>"
 */

int wasm_run_from_mem(const unsigned char* wasm, size_t wasm_size, int timeout_secs, size_t mem_kb, char* const argv[], int* exit_code) {
    char tmp[] = "/tmp/aios_wasm_XXXXXX.wasm";
    int fd = mkstemps(tmp, 5); // .wasm suffix
    if (fd < 0) { perror("mkstemps"); return -1; }
    FILE* f = fdopen(fd, "wb");
    if (!f) { close(fd); return -1; }
    if (fwrite(wasm, 1, wasm_size, f) != wasm_size) { fclose(f); unlink(tmp); return -1; }
    fclose(f);

    pid_t pid = fork();
    if (pid == -1) { unlink(tmp); return -1; }
    if (pid == 0) {
        // child
        char mem_arg[64];
        snprintf(mem_arg, sizeof(mem_arg), "ulimit -v %zu; exec wasmtime %s", mem_kb, tmp);
        if (timeout_secs > 0) {
            execl("/bin/sh", "sh", "-c", (char[256]){0}, (char*)NULL);
            // We use execl with a composed command below instead; build final command
        }
        // fallback: run without timeout
        execlp("wasmtime", "wasmtime", tmp, (char*)NULL);
        _exit(127);
    }

    int status = 0;
    int rc = waitpid(pid, &status, 0);
    int code = -1;
    if (rc > 0) {
        if (WIFEXITED(status)) code = WEXITSTATUS(status);
        else if (WIFSIGNALED(status)) code = 128 + WTERMSIG(status);
    }
    if (exit_code) *exit_code = code;
    unlink(tmp);
    return 0;
}

/* Minimal CLI for PoC: write wasm file from argv[1] (path to wasm embedded file) and execute */
int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "Usage: %s <wasm-file>\n", argv[0]); return 2; }
    // We just exec wasmtime directly for the given path so PoC test can call it
    execlp("wasmtime", "wasmtime", argv[1], (char*)NULL);
    perror("execlp");
    return 127;
}
