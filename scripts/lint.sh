#!/usr/bin/env bash
set -euo pipefail

if ! command -v docker >/dev/null 2>&1; then
  echo "Docker is required to run lint locally" >&2
  exit 1
fi

echo "[lint] Running cppcheck in Docker"
docker run --rm -v "$PWD":/workspace -w /workspace ubuntu:22.04 bash -lc "export DEBIAN_FRONTEND=noninteractive; apt-get update; apt-get install -y cppcheck; cppcheck --enable=all --suppress=missingIncludeSystem -I include src tests || true"

echo "[lint] Completed (cppcheck findings above)."