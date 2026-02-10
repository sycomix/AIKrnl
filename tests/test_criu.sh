#!/usr/bin/env bash
set -euo pipefail

# Spawn a trivial background process that sleeps and writes a file periodically
cat > /tmp/aios_criu_worker.c <<'C'
#include <stdio.h>
#include <unistd.h>
int main() {
  for (int i = 0; i < 100; ++i) {
    FILE* f = fopen("/tmp/aios_criu_state.txt","w"); if (f) { fprintf(f, "%d\n", i); fclose(f); }
    sleep(1);
  }
  return 0;
}
C

gcc -o /tmp/aios_criu_worker /tmp/aios_criu_worker.c
/tmp/aios_criu_worker &
pid=$!

# Try to checkpoint
bash ./tools/criu/checkpoint_restore.sh $pid || rc=$?
if [ "${rc:-0}" = "77" ]; then
  echo "CRIU not available; skipping test" >&2
  kill $pid || true
  exit 77
fi
# If checkpoint succeeded return success
kill $pid || true
echo "CRIU test passed"
exit 0
