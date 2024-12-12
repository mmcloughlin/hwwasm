#pragma once

#include <wasm_simd128.h>

typedef v128_t uint8x16_t;
typedef v128_t uint32x4_t;

// vaddq_u32

static inline uint32x4_t vaddq_u32(uint32x4_t a, uint32x4_t b) {
    return wasm_i32x4_add(a, b);
}

// vdupq_n_u32

static inline uint32x4_t vdupq_n_u32(uint32_t value) {
    return wasm_u32x4_splat(value);
}

// vld1q_u32

static inline uint32x4_t vld1q_u32(uint32_t const *ptr) {
    return wasm_v128_load(ptr);
}

// vst1q_u32

static inline void vst1q_u32(uint32_t *ptr, uint32x4_t val) {
    wasm_v128_store(ptr, val);
}

// vrev32q_u8

uint8x16_t __intrinsic_vrev32q_u8(uint8x16_t vec);

static inline uint8x16_t vrev32q_u8(uint8x16_t vec) {
    return __intrinsic_vrev32q_u8(vec);
}

// vgetq_lane_u32

static inline uint32_t vgetq_lane_u32(uint32x4_t v, const int lane) {
    switch (lane) {
        case 0:
            return wasm_u32x4_extract_lane(v, 0);
        case 1:
            return wasm_u32x4_extract_lane(v, 1);
        case 2:
            return wasm_u32x4_extract_lane(v, 2);
        case 3:
            return wasm_u32x4_extract_lane(v, 3);
        default:
            __builtin_unreachable();
    }
}

// vreinterpretq_u32_u8

static inline uint32x4_t vreinterpretq_u32_u8(uint8x16_t a) {
    return a;
}

// vreinterpretq_u8_u32

static inline uint8x16_t vreinterpretq_u8_u32(uint32x4_t a) {
    return a;
}

// vsha1cq_u32

uint32x4_t __intrinsic_vsha1cq_u32(uint32x4_t hash_abcd, uint32x4_t hash_e, uint32x4_t wk);

static inline uint32x4_t vsha1cq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk) {
    return __intrinsic_vsha1cq_u32(hash_abcd, wasm_u32x4_splat(hash_e), wk);
}

// vsha1pq_u32

uint32x4_t __intrinsic_vsha1pq_u32(uint32x4_t hash_abcd, uint32x4_t hash_e, uint32x4_t wk);

static inline uint32x4_t vsha1pq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk) {
    return __intrinsic_vsha1pq_u32(hash_abcd, wasm_u32x4_splat(hash_e), wk);
}

// vsha1mq_u32

uint32x4_t __intrinsic_vsha1mq_u32(uint32x4_t hash_abcd, uint32x4_t hash_e, uint32x4_t wk);

static inline uint32x4_t vsha1mq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk) {
    return __intrinsic_vsha1mq_u32(hash_abcd, wasm_u32x4_splat(hash_e), wk);
}

// vsha1h_u32

uint32x4_t __intrinsic_vsha1h_u32(uint32x4_t hash_e);

static inline uint32_t vsha1h_u32(uint32_t hash_e) {
    return wasm_u32x4_extract_lane(__intrinsic_vsha1h_u32(wasm_u32x4_splat(hash_e)), 0);
}

// vsha1su0q_u32

uint32x4_t __intrinsic_vsha1su0q_u32(uint32x4_t w0_3, uint32x4_t w4_7, uint32x4_t w8_11);

static inline uint32x4_t vsha1su0q_u32(uint32x4_t w0_3, uint32x4_t w4_7, uint32x4_t w8_11) {
    return __intrinsic_vsha1su0q_u32(w0_3, w4_7, w8_11);
}

// vsha1su1q_u32

uint32x4_t __intrinsic_vsha1su1q_u32(uint32x4_t tw0_3, uint32x4_t w12_15);

static inline uint32x4_t vsha1su1q_u32(uint32x4_t tw0_3, uint32x4_t w12_15) {
    return __intrinsic_vsha1su1q_u32(tw0_3, w12_15);
}
