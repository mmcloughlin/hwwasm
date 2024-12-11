#!/usr/bin/env python3

import os
import json


ENABLED_INTRINSICS = {
    "vld1q_u32",
    "vst1q_u32",
    "vaddq_u32",
    "vdupq_n_u32",
    "vgetq_lane_u32",
    "vreinterpretq_u32_u8",
    "vreinterpretq_u8_u32",
    "vrev32q_u8",
    "vsha1cq_u32",
    "vsha1h_u32",
    "vsha1mq_u32",
    "vsha1pq_u32",
    "vsha1su0q_u32",
    "vsha1su1q_u32",
}


def data_path(name):
    return os.path.join(os.getenv("HWWASM_DATA_DIR"), name)


def read_intrinsics_database():
    with open(data_path("intrinsics.json")) as f:
        return json.load(f)


def generate_c_header_declarations(intrinsics):
    for intrinsic in intrinsics:
        if intrinsic["name"] not in ENABLED_INTRINSICS:
            continue

        params = ", ".join(intrinsic["arguments"])
        args = ", ".join(arg.split()[-1] for arg in intrinsic["arguments"])
        ret = intrinsic["return_type"]["value"]
        name = intrinsic["name"]
        builtin_name = f"__intrinsic_{name}"

        print(f"// {name}")
        print()
        print(f"{ret} {builtin_name}({params});")
        print()
        print(f"static inline {ret} {name}({params}) {{ return {builtin_name}({args}); }}")
        print()


def main():
    intrinsics = read_intrinsics_database()
    generate_c_header_declarations(intrinsics)


if __name__ == "__main__":
    main()
