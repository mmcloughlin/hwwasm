#pragma once

#include <wasm_simd128.h>

typedef v128_t uint8x16_t;
typedef v128_t uint32x4_t;

uint32x4_t vaddq_u32(uint32x4_t a, uint32x4_t b);

uint32x4_t vdupq_n_u32(uint32_t value);

static inline uint32x4_t vld1q_u32(uint32_t const *ptr) {
    return wasm_v128_load(ptr);
}

static inline void vst1q_u32(uint32_t *ptr, uint32x4_t val) {
    wasm_v128_store(ptr, val);
}

uint8x16_t vrev32q_u8(uint8x16_t vec);

uint32_t vgetq_lane_u32(uint32x4_t v, const int lane);

uint32x4_t vreinterpretq_u32_u8(uint8x16_t a);

uint8x16_t vreinterpretq_u8_u32(uint32x4_t a);

uint32x4_t vsha1cq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk);

uint32x4_t vsha1pq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk);

uint32x4_t vsha1mq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk);

uint32_t vsha1h_u32(uint32_t hash_e);

uint32x4_t vsha1su0q_u32(uint32x4_t w0_3, uint32x4_t w4_7, uint32x4_t w8_11);

uint32x4_t vsha1su1q_u32(uint32x4_t tw0_3, uint32x4_t w12_15);
