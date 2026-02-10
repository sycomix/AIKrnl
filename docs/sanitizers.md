# Sanitizers (ASAN / TSAN)

This document explains how to run AddressSanitizer (ASAN) and ThreadSanitizer (TSAN) locally and how the CI sanitizer job works.

## Local runs (WSL / Docker)

- In WSL or an Ubuntu Docker container, install clang and run the provided script:

  ```bash
  sudo apt-get update && sudo apt-get install -y clang
  bash ./scripts/ci-sanitizers.sh
  ```

- The script runs `make test-asan` and `make test-tsan`. Logs are written to `sanitizer-asan.log` and `sanitizer-tsan.log` in the project root when run via the CI workflow or the script.

## CI behavior

- The CI workflow (`.github/workflows/ci.yml`) runs the sanitizers as a report-only job. It will not block merges initially.
- If a sanitizer run fails, its logs are uploaded as artifacts for debugging (kept for a limited retention period).

## Reproducing failures

- Download the artifact from the failed job, inspect `sanitizer-*.log`, and re-run locally using the same `clang` toolchain:

  ```bash
  sudo apt-get install -y clang
  make test-asan 2>&1 | tee sanitizer-asan.log
  ```

## Promotion plan

- Initially non-gating. After stabilizing sanitizer results and reducing false positives, the project may promote sanitizers to a gating job that blocks merges.