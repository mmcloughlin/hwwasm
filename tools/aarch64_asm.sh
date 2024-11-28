#!/usr/bin/env bash

set -euxo pipefail

clang -x assembler --target=aarch64 -march=armv8-a+sha2 - -c -o /dev/stdout \
    | llvm-objdump - -d --section=.text
