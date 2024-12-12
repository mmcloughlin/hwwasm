CC=clang
CFLAGS=-Wall -Wextra -Werror -O3

WASM_CC=$(WASI_SDK_PATH)/bin/clang
WASM_CFLAGS=$(CFLAGS) -msimd128

TOOLS=test bench
BACKENDS=intrinsics generic

.PHONY: all
all: sha1_intrinsics.o.wat sha1_generic.o.wat wasm_arm_neon.o.wat

define binary_template
all: sha1_$(1)_$(2) sha1_$(1)_$(2).wasm
sha1_$(1)_$(2): sha1_$(1).o sha1_$(2).o
	$$(CC) $$(CFLAGS) -o $$@ $$^
sha1_$(1)_$(2).wasm: sha1_$(1).o.wasm sha1_$(2).o.wasm wasm_arm_neon.o.wasm
	$$(WASM_CC) $$(WASM_CFLAGS) -o $$@ $$^
endef

$(foreach backend,$(BACKENDS),\
	$(foreach tool,$(TOOLS),\
		$(eval $(call binary_template,$(backend),$(tool)))\
	)\
)

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
	$(RM) sha1_*_test sha1_*_bench *.o *.wasm *.wat
