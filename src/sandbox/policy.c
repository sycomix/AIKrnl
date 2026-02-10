#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * PoC Policy engine: simple allowlist of commands. Used to validate that a requested action is permitted.
 * In production this would be eBPF-based policy enforcement.
 */

int policy_check_command(const char* cmd) {
    if (!cmd) return 0; // default deny
    /* Allowed commands for PoC */
    const char* allowed[] = {"echo", "true", "wasmtime", NULL};
    for (int i = 0; allowed[i]; ++i) {
        size_t len = strlen(allowed[i]);
        if (strncmp(cmd, allowed[i], len) == 0 && (cmd[len] == '\0' || cmd[len] == ' ')) return 1;
    }
    return 0;
}

int policy_run_command(const char* cmd) {
    if (!policy_check_command(cmd)) {
        fprintf(stderr, "policy: command denied: %s\n", cmd);
        return -1;
    }
    return system(cmd);
}
