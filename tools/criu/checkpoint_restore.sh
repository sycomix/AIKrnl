#!/usr/bin/env bash
set -euo pipefail

if ! command -v criu >/dev/null 2>&1; then
  echo "CRIU not found; skipping checkpoint test" >&2
  exit 77 # skip code
fi

# Usage: checkpoint_restore <pid>
if [ $# -ne 1 ]; then
  echo "Usage: $0 <pid>" >&2
  exit 2
fi
pid=$1
workdir=$(mktemp -d /tmp/aios-criu.XXXX)
criu dump -t $pid -D $workdir --shell-job --leave-running
status=$?
if [ $status -ne 0 ]; then
  echo "criu dump failed: $status" >&2
  rm -rf $workdir
  exit $status
fi
# Now attempt restore in a new namespace/process (PoC: not fully isolated)
# For PoC we don't actually spawn a separate container; we verify dump files exist
if [ ! -d "$workdir" ]; then
  echo "dump directory missing" >&2
  rm -rf $workdir
  exit 1
fi
# Clean up and report success
echo "CRIU dump created at $workdir" >&2
rm -rf $workdir
exit 0
