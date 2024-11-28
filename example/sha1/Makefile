CC=clang
CFLAGS=-Wall -Wextra -Werror -O3

WASM_CC=$(WASI_SDK_PATH)/bin/clang
WASM_CFLAGS=$(CFLAGS) -msimd128

TEST_BINARY=sha1_test

.PHONY: all
all: $(TEST_BINARY) sha1.o sha1.o.wasm sha1.o.wat wasm_arm_neon.o.wasm wasm_arm_neon.o.wat

$(TEST_BINARY): sha1.o

$(TEST_BINARY).wasm: sha1.o.wasm wasm_arm_neon.o.wasm

%.wasm: %.c
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
	$(RM) $(TEST_BINARY) *.o *.wasm *.wat
