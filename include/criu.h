#ifndef CRIU_H
#define CRIU_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Dump process `pid` into directory `dir` using CRIU. Returns 0 on success,
 * 77 if CRIU isn't available (test-skip), or -1 on error.
 */
int criu_dump_pid(pid_t pid, const char* dir);

/**
 * Restore a process from `dir` using CRIU. Returns 0 on success, 77 if CRIU
 * isn't available (test-skip), or -1 on error. On success sets *out_pid to the
 * restored PID (if available) or -1 otherwise.
 */
int criu_restore_dir(const char* dir, pid_t* out_pid);

#ifdef __cplusplus
}
#endif

#endif /* CRIU_H */