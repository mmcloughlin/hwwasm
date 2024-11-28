#pragma once

#include <stdio.h>

#include "intrinsics.h"

static void print_u32x4(const uint32x4_t x) {
    printf("[%08x,%08x,%08x,%08x]", vgetq_lane_u32(x, 0), vgetq_lane_u32(x, 1),
           vgetq_lane_u32(x, 2), vgetq_lane_u32(x, 3));
}

static void trace_prefix(const char *file, int line) {
    printf("%s:%d:\t", file, line);
}

static void trace_u32x4(const char *name, const uint32x4_t x) {
    printf("%s =\t", name);
    print_u32x4(x);
    printf("\n");
}

#define TRACE_MSG(X)                      \
    do {                                  \
        trace_prefix(__FILE__, __LINE__); \
        printf(X "\n");                   \
    } while (0)

#define TRACE_U32X4(X)                    \
    do {                                  \
        trace_prefix(__FILE__, __LINE__); \
        trace_u32x4(#X, X);               \
    } while (0)
