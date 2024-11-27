#include "wasm_arm_neon.h"

uint32x4_t vaddq_u32(uint32x4_t a, uint32x4_t b) {
    return wasm_i32x4_add(a, b);
}

// TODO: uint32x4_t vdupq_n_u32(uint32_t value);
// TODO: uint32x4_t vld1q_u32(uint32_t const * ptr);
// TODO: void vst1q_u32(uint32_t * ptr, uint32x4_t val);
// TODO: uint8x16_t vrev32q_u8(uint8x16_t vec);
// TODO: uint32_t vgetq_lane_u32(uint32x4_t v, const int lane);
// TODO: uint32x4_t vreinterpretq_u32_u8(uint8x16_t a);
// TODO: uint8x16_t vreinterpretq_u8_u32(uint32x4_t a);
// TODO: uint32x4_t vsha1cq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk);
// TODO: uint32x4_t vsha1pq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk);
// TODO: uint32x4_t vsha1mq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk);
// TODO: uint32_t vsha1h_u32(uint32_t hash_e);
// TODO: uint32x4_t vsha1su0q_u32(uint32x4_t w0_3, uint32x4_t w4_7, uint32x4_t w8_11);
// TODO: uint32x4_t vsha1su1q_u32(uint32x4_t tw0_3, uint32x4_t w12_15);
