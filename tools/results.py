#!/usr/bin/env python3

import os
import json
import collections
from glob import glob


class Result(collections.namedtuple("Result", ["metadata", "runs"])):
    @property
    def name(self):
        return self.metadata["name"]

    def is_wip(self):
        return self.name == "wip"

    def native_run(self):
        return self.runs["native"]

    def validate(self):
        # Disallow WIP results
        assert not self.is_wip()

        # Git versions should not be dirty
        assert not self.metadata["hwwasm_git_version"].endswith("-dirty")
        assert not self.metadata["wasmtime_hwwasm_git_version"].endswith("-dirty")

        # Wasmtime version should be forked from baseline.
        assert self.metadata["wasmtime_hwwasm_version"].startswith(self.metadata["wasmtime_baseline_version"])

        # Wasmtime fork should be built from the git version hash.
        git_version = self.metadata["wasmtime_hwwasm_git_version"][:9]
        assert git_version in self.metadata["wasmtime_hwwasm_version"]

        # All runs should agree on parameters and final state.
        baseline = self.native_run()
        for run in self.runs.values():
            assert run["message_blocks"] == baseline["message_blocks"]
            assert run["iterations"] == baseline["iterations"]
            assert run["final_state"] == baseline["final_state"]


def read_result(path):
    data_files = glob("*.json", root_dir=path)

    # Metadata
    METADATA_FILE = "metadata.json"
    data_files.remove(METADATA_FILE)
    with open(os.path.join(path, METADATA_FILE)) as f:
        metadata = json.load(f)

    # Run results
    runs = {}
    for run_file in data_files:
        name, _ = os.path.splitext(run_file)
        with open(os.path.join(path, run_file)) as f:
            run = json.load(f)
            runs[name] = run

    return Result(metadata, runs)


def read_results(root_dir):
    results = []
    for name in os.listdir(root_dir):
        path = os.path.join(root_dir, name)
        if os.path.isdir(path):
            result = read_result(path)
            if not result.is_wip():
                results.append(result)
    results.sort(key=lambda r: r.metadata["timestamp"])
    return results


def report(results):
    print(f"| Commit | Change | vs. Native |")
    print(f"| --- | --- | --- |")
    for result in results:
        name = result.name
        native_run = result.native_run()
        intrinsics_run = result.runs["wasmtime_hwwasm"]
        scale = intrinsics_run["elapsed_ns"] / native_run["elapsed_ns"]
        commit = result.metadata["wasmtime_hwwasm_git_version"]
        commit_url = "https://github.com/mmcloughlin/hwwasmtime/commit/" + commit
        print(f"| [`{commit}`]({commit_url}) | `{name}` | x{scale:.2f} |")


def main():
    results = read_results("results")
    for result in results:
        result.validate()
    report(results)


if __name__ == "__main__":
    main()
