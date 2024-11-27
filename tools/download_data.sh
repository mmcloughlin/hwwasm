#!/usr/bin/env bash

set -euxo pipefail

# Setup data directory.
[[ -n "${HWWASM_DATA_DIR}" ]]

# Intrinsics database.
wget -O "${HWWASM_DATA_DIR}/intrinsics.json" \
    "https://developer.arm.com/architectures/instruction-sets/intrinsics/data/intrinsics.json"
