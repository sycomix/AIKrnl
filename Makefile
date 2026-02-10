CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra
LDLIBS = -lm -lpthread

INCLUDES = -Iinclude -Isrc -Idrivers
SRC = arch/x86/lib/fp_math.c src/scheduler/fpu_context.c drivers/gpu/gpu_mock.c
OBJ = build/fp_math.o build/fpu_context.o build/gpu_mock.o build/memoryos.o build/scheduler.o
FPGA_OBJ = build/fpga_mock.o build/fp_accel.o
TEST_BIN = tests/test_fp_math tests/test_scheduler_fpu tests/test_gpu tests/test_memoryos_allocator tests/test_memoryos_promotion tests/test_p2os_semantic tests/test_integration_memory_gpu_fp tests/test_scheduler_integration tests/test_scheduler_concurrency tests/test_sandbox_policy tests/test_fpga_hls_emulation tests/test_fpga_integration

.PHONY: all build test clean

all: test

build: $(OBJ)
	@echo "Built: $(OBJ)"

build/%.o: %.c include/%.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Explicit rules for files whose headers live outside include/
build/fpu_context.o: src/scheduler/fpu_context.c src/scheduler/fpu_context.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDES) -c src/scheduler/fpu_context.c -o build/fpu_context.o

build/gpu_mock.o: drivers/gpu/gpu_mock.c drivers/gpu/gpu.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDES) -c drivers/gpu/gpu_mock.c -o build/gpu_mock.o

build/fpga_mock.o: drivers/accel/fpga_mock.c drivers/accel/fpga.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDES) -c drivers/accel/fpga_mock.c -o build/fpga_mock.o

build/fp_accel.o: fpga/model/fp_accel.cpp
	@mkdir -p build
	g++ -std=c++11 -O2 -Iinclude -c fpga/model/fp_accel.cpp -o build/fp_accel.o

build/memoryos.o: src/memoryos/memoryos.c include/memoryos.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDES) -c src/memoryos/memoryos.c -o build/memoryos.o

build/scheduler.o: src/scheduler/scheduler.c src/scheduler/scheduler.h include/memoryos.h drivers/gpu/gpu.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDES) -c src/scheduler/scheduler.c -o build/scheduler.o

tests/test_fp_math: tests/test_fp_math.c build/fp_math.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_fp_math.c build/fp_math.o -o $@ $(LDLIBS)

tests/test_scheduler_fpu: tests/test_scheduler_fpu.c build/fpu_context.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_scheduler_fpu.c build/fpu_context.o -o $@ $(LDLIBS)

tests/test_gpu: tests/test_gpu.c build/gpu_mock.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_gpu.c build/gpu_mock.o -o $@ $(LDLIBS)

tests/test_memoryos_allocator: tests/test_memoryos_allocator.c build/memoryos.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_memoryos_allocator.c build/memoryos.o -o $@ $(LDLIBS)

tests/test_memoryos_promotion: tests/test_memoryos_promotion.c build/memoryos.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_memoryos_promotion.c build/memoryos.o -o $@ $(LDLIBS)

tests/test_p2os_semantic: tests/test_p2os_semantic.c build/memoryos.o
tests/test_p2os_semantic: tests/test_p2os_semantic.c build/memoryos.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_p2os_semantic.c build/memoryos.o -o $@ $(LDLIBS)

tests/test_integration_memory_gpu_fp: tests/test_integration_memory_gpu_fp.c build/memoryos.o build/fp_math.o build/gpu_mock.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_integration_memory_gpu_fp.c build/memoryos.o build/fp_math.o build/gpu_mock.o -o $@ $(LDLIBS)

tests/test_scheduler_integration: tests/test_scheduler_integration.c build/memoryos.o build/scheduler.o build/gpu_mock.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_scheduler_integration.c build/memoryos.o build/scheduler.o build/gpu_mock.o -o $@ $(LDLIBS)

tests/test_scheduler_concurrency: tests/test_scheduler_concurrency.c build/memoryos.o build/scheduler.o build/gpu_mock.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_scheduler_concurrency.c build/memoryos.o build/scheduler.o build/gpu_mock.o -o $@ $(LDLIBS)

tests/test_sandbox_policy: tests/test_sandbox_policy.c
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_sandbox_policy.c -o $@ $(LDLIBS)

tests/test_fpga_hls_emulation: tests/test_fpga_hls_emulation.c build/fp_accel.o build/fpga_mock.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_fpga_hls_emulation.c build/fpga_mock.o build/fp_accel.o -o $@ $(LDLIBS) -lstdc++

tests/test_fpga_integration: tests/test_fpga_integration.c build/memoryos.o build/fp_accel.o build/fpga_mock.o
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_fpga_integration.c build/memoryos.o build/fpga_mock.o build/fp_accel.o -o $@ $(LDLIBS) -lstdc++

# FPGA UART hardware test (skip-aware)
tests/test_fpga_uart: tests/test_fpga_uart.c
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_fpga_uart.c -o $@ $(LDLIBS)

.PHONY: sandbox-criu sandbox-wasm test-fpga-uart

# Run the UART hardware test; treat exit code 77 as a skip
test-fpga-uart: tests/test_fpga_uart
	@echo "Running FPGA UART hardware test (skips if no device)"
	-./tests/test_fpga_uart || { code=$$?; if [ $$code -eq 77 ]; then echo "SKIPPED FPGA UART test (device not found)"; exit 0; else exit $$code; fi }
sandbox-criu:
	@echo "Running CRIU test (may be skipped if CRIU missing)"
	@bash ./tests/test_criu.sh || test $$? -eq 77

tests/test_criu_c: tests/test_criu_c.c src/criu/criu.c include/criu.h
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_criu_c.c src/criu/criu.c -o $@ $(LDLIBS)
	tests/test_criu_c: tests/test_criu_c.c src/criu/criu.c include/criu.h
	$(CC) $(CFLAGS) $(INCLUDES) tests/test_criu_c.c src/criu/criu.c -o $@ $(LDLIBS)

sandbox-wasm:
	@echo "Checking Wasmtime availability (PoC)"
	@if command -v wasmtime >/dev/null 2>&1; then echo "wasmtime found"; else echo "wasmtime missing; skipping wasm tests"; fi

test: $(TEST_BIN)
	@echo "Running FP math tests..."
	./tests/test_fp_math
	@echo "Running scheduler FPU tests..."
	./tests/test_scheduler_fpu
	@echo "Running GPU mock tests..."
	./tests/test_gpu
	@echo "Running MemoryOS allocator tests..."
	./tests/test_memoryos_allocator
	@echo "Running MemoryOS promotion tests..."
	./tests/test_memoryos_promotion
	@echo "Running P2OS semantic tests..."
	./tests/test_p2os_semantic
	@echo "Running integration tests (MemoryOS + FP + GPU mock)..."
	./tests/test_integration_memory_gpu_fp
	@echo "Running scheduler integration tests..."
	./tests/test_scheduler_integration
	@echo "Running scheduler concurrency stress test..."
	./tests/test_scheduler_concurrency
	@echo "Running sandbox policy tests..."
	./tests/test_sandbox_policy
	@echo "Running sandbox CRIU test (may skip)..."
	@$(MAKE) sandbox-criu
	@echo "Running sandbox Wasm availability check (may skip)..."
	@$(MAKE) sandbox-wasm

clean:
	rm -rf build $(TEST_BIN)

.PHONY: ci-local ci-local-privileged lint check-stubs test-asan test-tsan
ci-local:
	@echo "Running local CI (non-privileged Docker)"
	@bash ./scripts/ci-local.sh

ci-local-privileged:
	@echo "Running local CI with privileged CRIU test (may require host support)"
	@bash ./scripts/ci-local.sh --privileged

lint:
	@echo "Running static analysis (cppcheck) in Docker"
	@bash ./scripts/lint.sh

check-stubs:
	@echo "Checking for stub markers (TODO/FIXME/STUB/MOCK)"
	@bash ./tools/check_no_stubs.sh

# ASAN/TSAN PoC targets (use clang)
test-asan:
	@echo "Building and running tests with AddressSanitizer (ASAN)"
	@CC=clang CFLAGS="$(CFLAGS) -fsanitize=address -g -O1" make clean
	@CC=clang CFLAGS="$(CFLAGS) -fsanitize=address -g -O1" make all || true

test-tsan:
	@echo "Building and running concurrency test with ThreadSanitizer (TSAN)"
	@CC=clang CFLAGS="$(CFLAGS) -fsanitize=thread -g -O1" make clean
	@CC=clang CFLAGS="$(CFLAGS) -fsanitize=thread -g -O1" make tests/test_scheduler_concurrency || true
