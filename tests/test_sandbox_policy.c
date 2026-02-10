#include <stdio.h>
#include "../src/sandbox/policy.c"

int main(void) {
    if (policy_check_command("echo hi") != 1) { printf("policy should allow echo\n"); return 1; }
    if (policy_check_command("rm -rf /") != 0) { printf("policy should deny rm\n"); return 1; }
    if (policy_run_command("echo hello >/tmp/aios_policy_test.txt") != 0) { printf("policy run echo failed\n"); return 1; }
    if (policy_run_command("rm -rf /") == 0) { printf("policy allowed rm unexpectedly\n"); return 1; }
    printf("Policy tests passed\n");
    return 0;
}
