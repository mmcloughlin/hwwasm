LATEXMKFLAGS=-shell-escape

.PHONY: all
all: build

.PHONY: build
build: snippets
	latexmk $(LATEXMKFLAGS) -pdf report

.PHONY: preview
preview: LATEXMKFLAGS+=-pv
preview: build

.PHONY: snippets
snippets:
	./snippet.sh "Rounds 28-31" "vsha1su0q_u32" ../example/sha1/sha1_intrinsics.c > snippets/sha1_intrinsics_rounds.c

.PHONY: clean
clean:
	latexmk -C
	$(RM) *.run.xml *.bbl
