LATEXMKFLAGS=-shell-escape

.PHONY: all
all: build

.PHONY: build
build: snippets results version
	latexmk $(LATEXMKFLAGS) -pdf report

.PHONY: preview
preview: LATEXMKFLAGS+=-pv
preview: build

.PHONY: website
website: build
	$(RM) -r website
	mkdir -p website
	cp report.pdf website/hwwasm.pdf

.PHONY: snippets
snippets:
	./snippet.sh "Rounds 28-31" "vsha1su0q_u32" ../example/sha1/sha1_intrinsics.c > snippets/sha1_intrinsics_rounds.c
	./snippet.sh "__intrinsic_vsha1h_u32" "}"  ../example/sha1/wasm_arm_neon.h > snippets/__intrinsic_vsha1h_u32.h
	./snippet.sh "inline uint32x4_t vaddq_u32" "}"  ../example/sha1/wasm_arm_neon.h > snippets/vaddq_u32.h

.PHONY: results
results:
	../tools/results.py latex_metrics ../results/ > metrics.gen.tex
	../tools/results.py latex_intrinsics_table ../results/ > intrinsicstable.gen.tex
	../tools/results.py latex_refined_table ../results/ > refinedtable.gen.tex
	../tools/results.py latex_results_table ../results/ > resultstable.gen.tex

.PHONY: version
version:
	./version.sh > version.gen.tex

.PHONY: clean
clean:
	latexmk -C
	$(RM) *.run.xml *.bbl *.gen.tex
