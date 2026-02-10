# TODO — Progress vs `build.md` ✅

## Summary
- Progress: Repo scaffold, CI skeleton, FP math prototype (AVX‑512-aware + fallback), scheduler FPU context, GPU driver *mock*, MemoryOS (zero-copy pool + allocator + promotion heuristics), P2OS semantic index (embedding store + query), scheduler that picks hot MemoryOS allocations and runs GPU in-place work. Comprehensive unit/integration/concurrency tests added and passing locally. 🧪
- Gap: Kernel-level Ring‑0 implementations, eBPF/Wasm sandboxing, CRIU checkpoint/restore, real GPU backend, production hardening, benchmarks, and formal acceptance criteria from `build.md` are still outstanding. ⚠️

---

## Component-by-component TODOs (derived from `build.md`)

1) Ring‑0 Vectorized FP Engine (target: `arch/x86/lib/fp_math.c`) — Status: Prototype ✅
   - Done: `arch/x86/lib/fp_math.c` with AVX‑512 runtime check + scalar fallback; tests & microcases. ✅
   - TODO (High): implement fully optimized kernels, microbenchmarks, assembly tuned code paths, formal FP regression tests, safety reviews. Acceptance: deterministic results, performance target met, CI benchmarks.

2) Scheduler FPU Context Hooks — Status: Prototype ✅
   - Done: `src/scheduler/fpu_context.{c,h}` with save/restore + tests. ✅
   - TODO (Medium): integrate with real kernel context switch or kernel module; test with preemptive scheduling; add fuzzing. Acceptance: correct state across context switches under stress.

3) GPU Driver Framework (`drivers/gpu/*`) — Status: Mock implemented ✅
   - Done: API (`gpu_allocate_memory`, `gpu_execute_task`, `gpu_execute_task_inplace`) and `drivers/gpu/gpu_mock.c`. Integration tests with MemoryOS + scheduler. ✅
   - TODO (High): add vendor backends (CUDA/ROCm), async submission, DMA zero-copy, device registration, error handling and interchange tests. Acceptance: real GPU integration test and performance metrics.

4) MemoryOS & P2OS Semantic FS — Status: Functional prototype ✅
   - Done: `include/memoryos.h`, `src/memoryos/memoryos.c` (bitmap allocator, metadata, promotion, P2OS index/query/get_data), tests, concurrency safety (pthread mutex), `memoryos_pick_hot` + `memoryos_reset_heat`. ✅
   - TODO (High): persistent storage, ANN index (Faiss/hnsw), eviction policies, priority queues for promotion, performance benchmarks, cross-process zero-copy (mmap/SHM), encryption/ACLs. Acceptance: integrated semantic queries, memstore persistence, performance within targets.

5) eBPF & Wasm Sandboxing + CRIU checkpoint/restore — Status: PoC implemented (prototype) ✅
   - Done (PoC): `docs/sandbox.md` design doc; `src/sandbox/wasm_runner.c` Wasmtime-based PoC runner; `src/sandbox/policy.c` policy engine + `tests/test_sandbox_policy.c`; CRIU helper `tools/criu/checkpoint_restore.sh`, C wrapper `include/criu.h` and `src/criu/criu.c`, tests `tests/test_criu.sh` and `tests/test_criu_c.c`; Makefile targets for sandbox tests; local Docker-based CI helper `scripts/ci-local.sh` and Makefile targets `ci-local`/`ci-local-privileged`. Sandbox and CRIU tests skip gracefully when system tools are unavailable. ✅
   - TODO (High): harden sandbox (embed libwasmtime, add seccomp/cgroups, integrate kernel eBPF enforcement), verify CRIU restore end-to-end on privileged runners, and add a CI job (privileged or org-level) to run CRIU restore verification. Acceptance: reproducible sandbox tests, policy enforcement verified, CRIU checkpoint/restore verified on supported CI runner (or documented local privileged run).

6) Stateful Pause & Checkpointing (CRIU) — Status: PoC implemented ✅
   - Done (PoC): CRIU wrapper (`include/criu.h`, `src/criu/criu.c`), helper script (`tools/criu/checkpoint_restore.sh`), tests (`tests/test_criu.sh`, `tests/test_criu_c.c`) and docs updated (`docs/sandbox.md`). Tests are resilient and skip when CRIU isn't available. ✅
   - TODO (Medium): add robust restore verification (compare restored process state to pre-dump state), support privileged CI runs or provide documented local privileged run instructions, and detail kernel capability requirements. Acceptance: successful checkpoint/restore verified in CI or documented local privileged run.

7) CI, Manifests, ai-rules.md Compliance — Status: Initial scaffold ✅
   - Done: `.github/workflows/ci.yml`, `manifest.txt`, `run.sh`, pinned tool hints, `Makefile`, tests included. ✅
   - TODO (High): add sanitizers (TSAN/ASAN), static analysis, stricter pinned dependency manifests for used toolchains, reproducible builds, release artifacts. Acceptance: CI gates with sanitizers and deterministic build matrix.

8) Concurrency & Stress Testing — Status: Implemented ✅
   - Done: thread-safe MemoryOS (mutex), `tests/test_scheduler_concurrency.c` stress test. ✅
   - TODO (Medium): add TSAN CI job, longer stress runs, instrumentation, stronger invariants checks (no overlaps, reserved semantics). Acceptance: no races reported by TSAN, CI stress job passes.

9) Documentation & Governance — Status: Basic ✅
   - Done: `README.md`, `CONTRIBUTING.md`, `LICENSE`. ✅
   - TODO (Medium): design docs, API contracts, interface stability policy, security policy. Acceptance: top-level design documents + API reference in repo.

---

## Immediate prioritized tasks (next sprint)
7. (High) CI hardening: add sanitizers (TSAN/ASAN) and static analysis. Status: PoC local targets and scripts added (`scripts/ci-sanitizers.sh`, `make test-asan`, `make test-tsan`, `make lint`, `make check-stubs`). Next: integrate sanitizer jobs into CI runners (or run in Docker-based local CI), add TSAN/ASAN gating in PR checks, and interpret/address findings. (Estimate: S–M)
2. (High) Add async GPU backend stub (simulate async execution + failure modes) and tests. (Estimate: M)
3. (High) Add performance microbenchmarks for FP engine and MemoryOS (document baseline and targets). (Estimate: M)
4. (High) Harden eBPF/Wasm sandbox (embed libwasmtime, add seccomp/cgroups, integrate eBPF enforcement; add robust policy tests). (Estimate: L)
5. (Medium) Add persistent semantics and ANN index to P2OS (prototype using on-disk store + HNSW). (Estimate: L)
6. (Medium) Kernel baseline decision & plan for Ring‑0 integration (choose Linux tag/fork; add submodule or repo layout). (Estimate: M)
7. (Low) Packaging & reproducible release artifacts (deterministic builds, release script). (Estimate: M)

---

## Acceptance criteria (per `ai-rules.md`) ✍️
- Every delivered component includes: pinned manifests, runnable tests (unit + integration + failure cases), and a one-line run script. ✅
- Reproducible builds on CI and local. ✅ (scaffolded) -> TODO: fully implemented
- Static analysis and sanitizers included in CI (ASAN/TSAN). ❌
- No stubs on critical subsystems in production branches — prototypes allowed on feature branches only. ❌ (many prototypes exist; must harden before merge to main)

---

## How to run tests locally
- Build & run all tests: `make all && make test` or `./run.sh`
- Run single test: `./tests/test_scheduler_concurrency`

---

## Notes & Risks
- Kernel/Ring‑0 work is high risk and requires hardware and careful validation; do not attempt without kernel baseline and CI harness. ⚠️
- GPU vendor integration requires hardware access (CUDA/ROCm) and licensing considerations. ⚠️
- Concurrency bugs can still exist; add TSAN into CI ASAP. ⚠️

### FPGA (Cyclone 10 LP) — hardware ready ✅
- You have the Cyclone 10 LP dev kit (USB‑UART). Plan to move the project to your WinTel system with Quartus to perform HLS and synthesis locally.

FPGA migration checklist (WinTel + Quartus)
- Pre-migration: run `make test-fpga-emul` on current system to verify behavioral emulation. ✅
- On WinTel: install Intel Quartus (and Intel HLS if desired). Document the exact Quartus version in `fpga/quartus/README.md`.
- Synthesis flow: run `make synth-fpga` (skeleton) which will invoke the HLS/Quartus steps (local only). Generate bitstream and program Cyclone 10 LP via USB/JTAG.
- Host integration: use simple USB‑UART (e.g., `/dev/ttyUSB0`, 115200/8/N/1) and the host UART test program to send/receive buffers and trigger kernels.

Tasks to complete after migration
1. Add HLS wrapper and Quartus project skeleton (`fpga/quartus/`) for Cyclone 10 LP. (Estimate: M)
2. Implement host UART transfer tool (termios) and test protocol for program → send buffer → run → receive results. (Estimate: S)
3. Add hardware-in-loop validation (`tests/test_fpga_integration` run against programmed board) and documentation for programming and verification. (Estimate: S–M)
4. Add performance microbenchmarks and document expected speedups vs CPU fallback. (Estimate: M)

Acceptance criteria (post-migration)
- Quartus synthesis produces a bitstream that successfully programs the dev board. ✅
- Hardware-in-loop `tests/test_fpga_integration` passes using the physical board and USB‑UART protocol. ✅
- Documentation in `fpga/quartus/README.md` contains exact steps, required Quartus options, and any board pin/constraint notes. ✅

---

If you confirm, I can (pick one):
- open a PR with the current branch and CI changes so remote runners validate (recommended), or
- start on the TSAN/ASAN CI job right away, or
- begin a PoC for the eBPF/Wasm sandbox.

Pick one and I’ll prepare the next sprint plan with tasks broken down and estimates. 🔧✨