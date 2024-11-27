CC=clang
CFLAGS=-Wall -Wextra -Werror -O3

WASM_CC=$(WASI_SDK_PATH)/bin/clang
WASM_CFLAGS=$(CFLAGS) -msimd128

.PHONY: all
all: sha1.o sha1.wasm sha1.wat

%.wasm: %.c
	$(WASM_CC) $(WASM_CFLAGS) -c -o $@ $<

%.wat: %.wasm
	wasm2wat $< -o $@

.PHONY: clean
clean:
	$(RM) *.o *.wasm
