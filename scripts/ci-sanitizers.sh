#!/usr/bin/env bash
set -euo pipefail

# Run ASAN and TSAN test runs inside Ubuntu Docker
if ! command -v docker >/dev/null 2>&1; then
  echo "Docker is required to run sanitizers locally" >&2
  exit 1
fi

echo "[sanitizers] Running ASAN build/test in Docker"
docker run --rm -v "$PWD":/workspace -w /workspace ubuntu:22.04 bash -lc "export DEBIAN_FRONTEND=noninteractive; apt-get update; apt-get install -y build-essential clang make gcc; CC=clang CFLAGS='-std=c11 -O1 -g -fsanitize=address' make clean; CC=clang CFLAGS='-std=c11 -O1 -g -fsanitize=address' make all"

echo "[sanitizers] Running TSAN build/test in Docker"
docker run --rm -v "$PWD":/workspace -w /workspace ubuntu:22.04 bash -lc "export DEBIAN_FRONTEND=noninteractive; apt-get update; apt-get install -y build-essential clang make gcc; CC=clang CFLAGS='-std=c11 -O1 -g -fsanitize=thread' make clean; CC=clang CFLAGS='-std=c11 -O1 -g -fsanitize=thread' make tests/test_scheduler_concurrency"

echo "[sanitizers] Completed"