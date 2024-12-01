#!/usr/bin/env python3

# See: https://github.com/llvm/llvm-project/blob/llvmorg-17.0.1/llvm/utils/chunk-print-before-all.py

import sys
import re
import logging
from dataclasses import dataclass
import argparse
import difflib
import os.path
import itertools


@dataclass
class PassIdentifier():
    name: str
    description: str


class PassOutput():
    def __init__(self, *, index, ident, lines):
        self.index = index
        self.ident = ident
        self.lines = lines

    @property
    def unique_id(self):
        return f"{self.index:03d}-{self.ident.name}"


def parse_pass_ident(line):
    # Is this a pass divider at all?
    if "*** IR Dump" not in line:
        return None

    # opt mid-end pass
    #
    #   "*** IR Dump After Annotation2MetadataPass on [module] ***"
    m = re.search(r'\*\*\* IR Dump After ((.+)\s+on .+) \*\*\*', line)
    if m:
        return PassIdentifier(name=m.group(2), description=m.group(1))

    # llc backend pass
    #
    #   "*** IR Dump After Scalarize Masked Memory Intrinsics (scalarize-masked-mem-intrin) ***"
    m = re.search(r'\*\*\* IR Dump After (.+) \((.+)\) \*\*\*', line)
    if m:
        return PassIdentifier(name=m.group(2), description=m.group(1))

    assert False, "expected to match pass identification"


def parse_pass_outputs(lines):
    pass_outputs = []
    current = None
    for line in lines:
        logging.debug("parse line: %s", line.strip())
        pass_ident = parse_pass_ident(line)
        if pass_ident:
            current = PassOutput(
                index=len(pass_outputs),
                ident=pass_ident,
                lines=[],
            )
            pass_outputs.append(current)
        else:
            current.lines.append(line)
    return pass_outputs


def write_pass_outputs(pass_outputs, *, output_directory):
    logging.info("write outputs to %s", output_directory)
    for pass_output in pass_outputs:
        path = os.path.join(output_directory, pass_output.unique_id + ".ll")
        with open(path, 'w') as f:
            for line in pass_output.lines:
                f.write(line)


def command_dump(opts):
    pass_outputs = parse_pass_outputs(opts.input)
    write_pass_outputs(pass_outputs, output_directory=opts.output)


def command_diff(opts):
    pass_outputs = parse_pass_outputs(opts.input)

    html_diff = difflib.HtmlDiff(wrapcolumn=opts.wrap)
    print("<html>", file=opts.output)
    print('''<html>
    <head>
        <style>
            body {font-family:monospace;}
            table.diff {border:medium;}
            .diff_header {background-color:#e0e0e0}
            td.diff_header {text-align:right}
            .diff_next {background-color:#c0c0c0}
            .diff_add {background-color:#aaffaa}
            .diff_chg {background-color:#ffff77}
            .diff_sub {background-color:#ffaaaa}
        </style>
    </head>
    <body>
    ''', file=opts.output)
    for (prev, curr) in itertools.pairwise(pass_outputs):
        if prev.lines == curr.lines:
            continue

        diff_table = html_diff.make_table(prev.lines, curr.lines, context=opts.context, numlines=opts.num_lines)
        print(f"<h2>{curr.unique_id}</h2>\n{diff_table}", file=opts.output)
    print("</body></html>", file=opts.output)


def main(args):
    # Options.
    parser = argparse.ArgumentParser(description='Chunk LLVM optimizer logs')
    parser.add_argument('--input', type=argparse.FileType('r'), default=sys.stdin, help="input LLVM optimizer log")
    subparsers = parser.add_subparsers()

    dump_parser = subparsers.add_parser("dump", help="dump intermediate ir")
    dump_parser.set_defaults(cmd=command_dump)
    dump_parser.add_argument('--output', required=True, metavar='DIR', help='output directory')

    diff_parser = subparsers.add_parser("diff", help="diff intermediate ir")
    diff_parser.set_defaults(cmd=command_diff)
    diff_parser.add_argument('--output', type=argparse.FileType('w'), default=sys.stdout, help="output HTML diff report")
    diff_parser.add_argument('--context', action=argparse.BooleanOptionalAction, default=False, help="display diff with context")
    diff_parser.add_argument('--num-lines', type=int, default=5, help="lines of context")
    diff_parser.add_argument('--wrap', type=int, default=120, help="wrap diff at column")

    opts = parser.parse_args(args)
    logging.basicConfig(level=logging.INFO)
    opts.cmd(opts)


if __name__ == "__main__":
    main(sys.argv[1:])
