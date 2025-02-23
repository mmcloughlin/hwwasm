#!/usr/bin/env bash

set -euxo pipefail

# Options.
function usage() {
    echo "Usage: ${0} -n <name>"
    exit 2
}

name="wip"
wasmtime_hwwasm_ref="HEAD"
clean="true"
while getopts "n:v:d" opt; do
    case "${opt}" in
        n) name="${OPTARG}" ;;
        v) wasmtime_hwwasm_ref="${OPTARG}" ;;
        d) clean="false" ;;
        *) usage ;;
    esac
done

[[ -n "${name}" ]]
[[ -n "${HWWASM_WASMTIME_DIR}" ]]

# Metadata helpers.
function json_new() {
    local file="${1}"
    echo '{}' >"${file}"
}

function json_set() {
    local file="${1}"
    local key="${2}"
    local value="${3}"
    jq '. += $ARGS.named' --arg "${key}" "${value}" "${file}" | sponge "${file}"
}

function git_version() {
    local repo="${1}"
    git -C "${repo}" describe --always --dirty --abbr=12 --exclude '*'
}

function git_commit_subject() {
    local repo="${1}"
    git -C "${repo}" log -1 --pretty='%s'
}

# Build SHA-1 example.
make -C example/sha1 clean all

# Ensure tests pass.
for test in example/sha1/sha1_*_test; do
    "${test}"
done

for test in example/sha1/sha1_*_test.wasm; do
    wasmtime run "${test}"
done

# Build wasmtime fork.
(
    cd "${HWWASM_WASMTIME_DIR}"
    git checkout "${wasmtime_hwwasm_ref}"
    if [[ "${clean}" == "true" ]]; then
        cargo clean
    fi
    cargo build --release --bin wasmtime
)
wasmtime_hwwasm="${HWWASM_WASMTIME_DIR}/target/release/wasmtime"

# Setup results directory.
timestamp=$(date -u '+%Y-%m-%dT%T')
output_directory="results/${timestamp}-${name}"
mkdir -p "${output_directory}"

# Metadata.
metadata_file="${output_directory}/metadata.json"
json_new "${metadata_file}"
json_set "${metadata_file}" "name" "${name}"
json_set "${metadata_file}" "timestamp" "${timestamp}"
hwwasm_git_version=$(git_version ".")
json_set "${metadata_file}" "hwwasm_git_version" "${hwwasm_git_version}"

# Benchmark: native.
./example/sha1/sha1_intrinsics_bench | tee "${output_directory}/native.json"
./example/sha1/sha1_generic_bench | tee "${output_directory}/native_generic.json"

# Benchmark: wasmtime baseline.
json_set "${metadata_file}" "wasmtime_baseline_version" "$(wasmtime --version)"
wasmtime run ./example/sha1/sha1_intrinsics_bench.wasm | tee "${output_directory}/wasmtime_baseline.json"
wasmtime run ./example/sha1/sha1_generic_bench.wasm | tee "${output_directory}/wasmtime_baseline_generic.json"

# Benchmark: wasmtime fork.
json_set "${metadata_file}" "wasmtime_hwwasm_git_version" "$(git_version "${HWWASM_WASMTIME_DIR}")"
json_set "${metadata_file}" "wasmtime_hwwasm_git_commit_subject" "$(git_commit_subject "${HWWASM_WASMTIME_DIR}")"
json_set "${metadata_file}" "wasmtime_hwwasm_version" "$("${wasmtime_hwwasm}" --version)"
"${wasmtime_hwwasm}" run ./example/sha1/sha1_intrinsics_bench.wasm | tee "${output_directory}/wasmtime_hwwasm.json"

# Debugging: generate explore output.
"${wasmtime_hwwasm}" explore example/sha1/sha1_intrinsics_test.wasm --output "${output_directory}/sha1_test.explore.html"
