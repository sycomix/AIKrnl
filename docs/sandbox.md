# Sandbox Design (PoC)

Goal: provide a secure, resource-limited sandbox for executing untrusted workloads (Wasm) and enforce kernel-level governance (eBPF-style policies). Also provide CRIU integration to checkpoint and restore sandboxed workloads.

Decisions (PoC stage)
- Wasm runtime: **Wasmtime CLI** for PoC (lightweight to install in CI). Long-term: link libwasmtime for embedding.
- eBPF policy module: start with a user-space policy engine that enforces a whitelist of allowed syscalls/commands for PoC. Later: compile/evaluate eBPF policies with libbpf or kernel hooks.
- CRIU: provide wrapper scripts that call `criu dump` and `criu restore` when available. PoC tests will skip if criu not installed.

Architecture (components)
- `sandbox/wasm_runner` (PoC): runs a Wasm binary using `wasmtime` with execution limits (timeout, memory via `ulimit`). Returns exit code and captures stdout/stderr.
- `sandbox/policy` (PoC): simple policy verifier library and CLI test harness to validate policy enforcement.
- `tools/criu/*`: small helper scripts to checkpoint and restore processes and basic CI tests that verify checkpoint/restore flow.

Security & resource limits (PoC)
- CPU time: use `timeout` wrapper in PoC. Replace with cgroups or seccomp in production.
- Memory: use `ulimit -v` in the wrapper for PoC. Replace with cgroups in production.
- Filesystem: Wasmtime `--mapdir` to control FS access; CLI mapdir allows exposing read-only dirs.

Acceptance criteria for PoC
- Reproducible sandbox tests: `tests/test_sandbox_wasm` runs a Wasm module and verifies output.
- Policy tests: `tests/test_sandbox_policy` validates allowed and denied operations.
- CRIU tests: `tests/test_criu` and `tests/test_criu_c` try to checkpoint and restore a small process; tests are skipped if CRIU is not available. In CI we'll install CRIU on ubuntu and run the test.

CI notes:
- The CRIU restore step may require a runner with appropriate kernel config and capabilities (ubuntu-latest should work in many cases). The job will install `criu` via apt and run the PoC. The test harness tolerates skips when CRIU is absent or unsupported.

Next steps
1. Implement PoC components and tests that run in GitHub Actions.  
2. Harden by embedding libwasmtime, adding seccomp/eBPF enforcement, and using cgroups for resource control.  
3. Design kernel integration plan for eBPF enforcement and CRIU in production environment.

Notes
- This PoC intentionally avoids direct kernel-level eBPF loading to remain runnable on unprivileged CI. The design documents the migration path to kernel enforcement.
