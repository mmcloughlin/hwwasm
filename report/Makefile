LATEXMKFLAGS=-shell-escape

.PHONY: all
all: build

.PHONY: build
build: snippets results
	latexmk $(LATEXMKFLAGS) -pdf report

.PHONY: preview
preview: LATEXMKFLAGS+=-pv
preview: build

.PHONY: snippets
snippets:
	./snippet.sh "Rounds 28-31" "vsha1su0q_u32" ../example/sha1/sha1_intrinsics.c > snippets/sha1_intrinsics_rounds.c
	./snippet.sh "__intrinsic_vsha1h_u32" "}"  ../example/sha1/wasm_arm_neon.h > snippets/__intrinsic_vsha1h_u32.h

.PHONY: results
results:
	../tools/results.py latex_metrics ../results/ > metrics.gen.tex
	../tools/results.py latex_iteration_table ../results/ > itertable.gen.tex

.PHONY: clean
clean:
	latexmk -C
	$(RM) *.run.xml *.bbl *.gen.tex
