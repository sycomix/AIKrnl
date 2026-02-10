#!/usr/bin/env bash
set -euo pipefail

# Local CI helper (Docker-based)
# Usage:
#  - Non-privileged run (default): ./scripts/ci-local.sh
#  - Privileged CRIU run (may require a compatible host kernel): ./scripts/ci-local.sh --privileged
# This script mounts the current working directory into /workspace and runs `make test` inside ubuntu:22.04.

PRIV=0
for arg in "$@"; do
  case "$arg" in
    --privileged|-p) PRIV=1 ;;
    --help|-h) echo "Usage: $0 [--privileged]"; exit 0 ;;
    *) echo "Unknown arg: $arg"; echo "Usage: $0 [--privileged]"; exit 2 ;;
  esac
done

if ! command -v docker >/dev/null 2>&1; then
  echo "Docker is not installed or not in PATH" >&2
  exit 1
fi

# Common base command for the non-privileged run
DOCKER_BASE=(docker run --rm -v "$PWD":/workspace -w /workspace ubuntu:22.04 bash -lc)

echo "[ci-local] Running full test suite inside Docker (non-privileged)"
"${DOCKER_BASE[@]}" "export DEBIAN_FRONTEND=noninteractive; apt-get update; apt-get install -y build-essential gcc make wasmtime; make clean; make all; make test"

if [ "$PRIV" -eq 1 ]; then
  echo "[ci-local] Running privileged CRIU test (requires --privileged Docker capability)"
  # Note: privileged runs may not work on all hosts (macOS/VM kernel config may block CRIU)
  docker run --rm --privileged -v "$PWD":/workspace -w /workspace ubuntu:22.04 bash -lc "export DEBIAN_FRONTEND=noninteractive; apt-get update; apt-get install -y build-essential gcc make criu; make sandbox-criu"
fi

echo "[ci-local] Done"
