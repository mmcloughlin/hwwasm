#!/usr/bin/env python3

import os
import sys
import json
import collections
import re
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


def markdown(results):
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


def latex_safe_name(name):
    # Replace numbers.
    NUMBERS = [
        ("0", "Zero"),
        ("1", "One"),
    ]
    for (digit, word) in NUMBERS:
        name = name.replace(digit, word)

    # Only keep letters
    return re.sub(r"[^a-zA-Z]", "", name)


def latex_metrics(results):
    for result in results:
        print (f"% {result.name}")
        for (name, run) in result.runs.items():
            for (other_name, other) in result.runs.items():
                if name == other_name:
                    continue
                scale = run["elapsed_ns"] / other["elapsed_ns"]
                parts = ["metric", result.name, name, "div", other_name]
                metric_name = "".join(part.title() for part in parts)
                command_name = latex_safe_name(metric_name)
                print(f"\\newcommand{{\\{command_name}}}{{{scale:.1f}x\\xspace}}")


def format_intrinsics_list(intrinsics):
    code_names = ["\\code{{{name}}}".format(name=name.replace("_", "\\_")) for name in intrinsics]
    return ", ".join(code_names)


def latex_iteration_table(results):
    NAME_TO_INSTRINSICS = {
        "sha1c": ["vsha1cq_u32"],
        "sha1pm": ["vsha1pq_u32", "vsha1mq_u32"],
        "sha1h": ["vsha1h_u32"],
        "sha1su0": ["vsha1su0q_u32"],
        "sha1su1": ["vsha1su1q_u32"],
        "vaddq_u32": ["vaddq_u32"],
        "vdupq_n_u32": ["vdupq_n_u32"],
        "vgetq_lane_u32": ["vgetq_lane_u32"],
        "vreinterpretq": ["vreinterpretq_u32_u8", "vreinterpretq_u8_u32"],
        "vrev32q_u8": ["vrev32q_u8"],
    }
    print("\\begin{tabular}{cl}")
    print("\\toprule")
    print("vs. Native & Added Intrinsics (including all above)\\\\")
    print("\\midrule")
    for result in results:
        name = result.name
        if name not in NAME_TO_INSTRINSICS:
            continue
        native_run = result.native_run()
        intrinsics_run = result.runs["wasmtime_hwwasm"]
        scale = intrinsics_run["elapsed_ns"] / native_run["elapsed_ns"]
        #commit = result.metadata["wasmtime_hwwasm_git_version"]
        #commit_url = "https://github.com/mmcloughlin/hwwasmtime/commit/" + commit
        intrinsics = format_intrinsics_list(NAME_TO_INSTRINSICS[name])
        print(f"{scale:.1f}x & {intrinsics} \\\\")
    print("\\bottomrule")
    print("\\end{tabular}")


COMMANDS = {
    "markdown": markdown,
    "latex_metrics": latex_metrics,
    "latex_iteration_table": latex_iteration_table,
}

def main():
    # Options
    command, path = sys.argv[1:]
    cmd = COMMANDS[command]

    # Read results
    results = read_results(path)
    for result in results:
        result.validate()

    # Execute command
    cmd(results)


if __name__ == "__main__":
    main()
