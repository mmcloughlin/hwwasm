CC=clang
CFLAGS=-Wall -Wextra -Werror -O3

WASM_CC=$(WASI_SDK_PATH)/bin/clang
WASM_CFLAGS=$(CFLAGS) -msimd128

.PHONY: all
all: \
	sha1_intrinsics_test sha1_intrinsics_bench sha1_generic_test sha1_generic_bench \
	sha1_intrinsics.o sha1_intrinsics.o.wasm sha1_intrinsics.o.wat wasm_arm_neon.o.wasm wasm_arm_neon.o.wat

sha1_intrinsics_test:   sha1_test.o   sha1_intrinsics.o
sha1_intrinsics_bench: 	sha1_bench.o  sha1_intrinsics.o
sha1_generic_test:      sha1_test.o   sha1_generic.o
sha1_generic_bench:     sha1_bench.o  sha1_generic.o

sha1_intrinsics_test sha1_intrinsics_bench sha1_generic_test sha1_generic_bench:
	$(CC) $(CFLAGS) -o $@ $^

%.wasm: %.o.wasm
	$(WASM_CC) $(WASM_CFLAGS) -o $@ $^

%.o.wasm: %.c
	$(WASM_CC) $(WASM_CFLAGS) -c -o $@ $<

%.wat: %.wasm
	wasm2wat $< -o $@

.PHONY: format
format:
	clang-format --style=file -i *.c *.h

.PHONY: clean
clean:
	$(RM) $(TEST_BINARY) $(BENCH_BINARY) *.o *.wasm *.wat
