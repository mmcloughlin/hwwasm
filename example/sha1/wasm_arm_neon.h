#pragma once

#include <wasm_simd128.h>

typedef v128_t uint8x16_t;
typedef v128_t uint32x4_t;

// vaddq_u32

uint32x4_t __intrinsic_vaddq_u32(uint32x4_t a, uint32x4_t b);

static inline uint32x4_t vaddq_u32(uint32x4_t a, uint32x4_t b) {
    return __intrinsic_vaddq_u32(a, b);
}

// vdupq_n_u32

uint32x4_t __intrinsic_vdupq_n_u32(uint32_t value);

static inline uint32x4_t vdupq_n_u32(uint32_t value) {
    return __intrinsic_vdupq_n_u32(value);
}

// vld1q_u32

uint32x4_t __intrinsic_vld1q_u32(uint32_t const *ptr);

static inline uint32x4_t vld1q_u32(uint32_t const *ptr) {
    return __intrinsic_vld1q_u32(ptr);
}

// vst1q_u32

void __intrinsic_vst1q_u32(uint32_t *ptr, uint32x4_t val);

static inline void vst1q_u32(uint32_t *ptr, uint32x4_t val) {
    return __intrinsic_vst1q_u32(ptr, val);
}

// vrev32q_u8

uint8x16_t __intrinsic_vrev32q_u8(uint8x16_t vec);

static inline uint8x16_t vrev32q_u8(uint8x16_t vec) {
    return __intrinsic_vrev32q_u8(vec);
}

// vgetq_lane_u32

uint32_t __intrinsic_vgetq_lane_u32(uint32x4_t v, const int lane);

static inline uint32_t vgetq_lane_u32(uint32x4_t v, const int lane) {
    return __intrinsic_vgetq_lane_u32(v, lane);
}

// vreinterpretq_u32_u8

uint32x4_t __intrinsic_vreinterpretq_u32_u8(uint8x16_t a);

static inline uint32x4_t vreinterpretq_u32_u8(uint8x16_t a) {
    return __intrinsic_vreinterpretq_u32_u8(a);
}

// vreinterpretq_u8_u32

uint8x16_t __intrinsic_vreinterpretq_u8_u32(uint32x4_t a);

static inline uint8x16_t vreinterpretq_u8_u32(uint32x4_t a) {
    return __intrinsic_vreinterpretq_u8_u32(a);
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
