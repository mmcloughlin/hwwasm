#!/usr/bin/env bash

set -euxo pipefail

prog="sha1"

# Compile to LLVM bitcode.
clang -Xclang -disable-O0-optnone -O0 -emit-llvm -fno-discard-value-names -c "${prog}.c" -o "${prog}.bc"

# Show the IR.
llvm-dis "${prog}.bc"

# Optimize
opt -O3 -print-after-all -print-module-scope "${prog}.bc" -o "${prog}.O3.bc" 2>"${prog}.O3.opt"

# Compile to native code at various levels.
for level in 0 1 2 3; do
    llc "-O${level}" "${prog}.O3.bc" -o "${prog}.llc.O${level}.s"
done

# Show the optimized IR.
llvm-dis "${prog}.O3.bc"

# Compile to native code.
llc -O3 "${prog}.O3.bc" -print-after-all -o "${prog}.llc.s" 2>"${prog}.llc.s.opt"

# Opts report.
../../tools/parse_llvm_opts.py --input "${prog}.llc.s.opt" diff --context --output "${prog}.llc.s.opt.html"
