#!/usr/bin/env bash

set -euxo pipefail

WASMTIME_VERSION="27.0.0"
cargo install "wasmtime-cli@${WASMTIME_VERSION}"
