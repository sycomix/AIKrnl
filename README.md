# AIOS — Prototype

This repository contains an initial scaffold and prototype implementations for the AI-native kernel described in `build.md`.

This initial commit focuses on creating a reproducible project scaffold that complies with `ai-rules.md` (pinned dependencies, a one-line run script, runnable tests, and CI), and a user-space prototype of the FP math engine with AVX-512-aware optimizations and portable fallbacks.

New additions in this sprint:
- Scheduler FPU context prototype (`src/scheduler/fpu_context.c`, tests in `tests/test_scheduler_fpu.c`) — simulates saving/restoring floating point environment.
- GPU driver API + mock implementation (`drivers/gpu/gpu.h`, `drivers/gpu/gpu_mock.c`, tests in `tests/test_gpu.c`).

Quickstart

- Build: `make all`
- Test: `make test`
- Run: `./run.sh`

Local CI (Docker):

- Run the entire test suite inside Docker (non-privileged):

  - `./scripts/ci-local.sh` or `make ci-local`

- Run the CRIU checkpoint/restore PoC (may require privileged container and host kernel support):

  - `./scripts/ci-local.sh --privileged` or `make ci-local-privileged`

Sanitizers & Static Analysis (local PoC):

- Run ASAN (AddressSanitizer) test run: `make test-asan` (uses clang to build and run tests with ASAN). Note: builds are O1 debug to keep sanitizer output useful.
- Run TSAN (ThreadSanitizer) concurrency test: `make test-tsan` (builds and runs `tests/test_scheduler_concurrency` under TSAN).
- Run static analysis: `make lint` (runs `cppcheck` inside Docker via `scripts/lint.sh`).
- Check for stub markers (TODO/FIXME/STUB/MOCK): `make check-stubs` runs `tools/check_no_stubs.sh`.

Notes:
- These sanitizer and lint targets are PoC and meant to be run locally or in privileged CI runners; results should be used to harden code before merging to production branches.
- CRIU requires kernel support and may not work inside every VM or macOS Docker host; the test scripts are resilient and skip when CRIU isn't available.

FPGA (Cyclone 10 LP) notes:

- We provide an HLS-friendly behavioral model and driver shim for early emulation and integration testing. Use `make test-fpga-emul` (or `make test` to run included FPGA emulation/integration tests) to validate correctness without hardware.
- For hardware-in-loop with your Cyclone 10 LP dev board (USB-UART): after synthesis and bitstream programming via Quartus, run the host program (to be added) which communicates with the board over the serial device (e.g., `/dev/ttyUSB0`) using 115200/8/N/1 settings. The current flow includes emulation-first tests and a Quartus skeleton in `fpga/quartus/` for later synthesis.

See `build.md` for the full design and `ai-rules.md` for mandatory rules and quality requirements.