LATEXMKFLAGS=-shell-escape

.PHONY: all
all: build

.PHONY: build
build:
	latexmk $(LATEXMKFLAGS) -pdf report

.PHONY: preview
preview: LATEXMKFLAGS+=-pv
preview: build

.PHONY: clean
clean:
	latexmk -C
	$(RM) *.run.xml *.bbl
