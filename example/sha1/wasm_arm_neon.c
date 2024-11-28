#include "wasm_arm_neon.h"

uint32x4_t vaddq_u32(uint32x4_t a, uint32x4_t b) {
    return wasm_i32x4_add(a, b);
}

uint32x4_t vdupq_n_u32(uint32_t value) {
    return wasm_u32x4_splat(value);
}

uint8x16_t vrev32q_u8(uint8x16_t vec) {
    const v128_t zero = wasm_i8x16_splat(0);
    return wasm_i8x16_shuffle(vec, zero, 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12);
}

uint32_t vgetq_lane_u32(uint32x4_t v, const int lane) {
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

uint32x4_t vreinterpretq_u32_u8(uint8x16_t a) {
    return a;
}

uint8x16_t vreinterpretq_u8_u32(uint32x4_t a) {
    return a;
}

#define SHA1_CHOOSE(X, Y, Z) (((Y ^ Z) & X) ^ Z)
#define SHA1_PARITY(X, Y, Z) (X ^ Y ^ Z)
#define SHA1_MAJORITY(X, Y, Z) ((X & Y) | ((X | Y) & Z))

#define SHA1_ROUND(F, I)                                                                    \
    do {                                                                                    \
        uint32_t f = F(b, c, d);                                                            \
        uint32_t t = __builtin_rotateleft32(a, 5) + f + e + wasm_u32x4_extract_lane(wk, I); \
        e = d;                                                                              \
        d = c;                                                                              \
        c = __builtin_rotateleft32(b, 30);                                                  \
        b = a;                                                                              \
        a = t;                                                                              \
    } while (0)

uint32x4_t vsha1cq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk) {
    uint32_t a = wasm_u32x4_extract_lane(hash_abcd, 0);
    uint32_t b = wasm_u32x4_extract_lane(hash_abcd, 1);
    uint32_t c = wasm_u32x4_extract_lane(hash_abcd, 2);
    uint32_t d = wasm_u32x4_extract_lane(hash_abcd, 3);
    uint32_t e = hash_e;

    SHA1_ROUND(SHA1_CHOOSE, 0);
    SHA1_ROUND(SHA1_CHOOSE, 1);
    SHA1_ROUND(SHA1_CHOOSE, 2);
    SHA1_ROUND(SHA1_CHOOSE, 3);

    return wasm_u32x4_make(a, b, c, d);
}

uint32x4_t vsha1pq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk) {
    uint32_t a = wasm_u32x4_extract_lane(hash_abcd, 0);
    uint32_t b = wasm_u32x4_extract_lane(hash_abcd, 1);
    uint32_t c = wasm_u32x4_extract_lane(hash_abcd, 2);
    uint32_t d = wasm_u32x4_extract_lane(hash_abcd, 3);
    uint32_t e = hash_e;

    SHA1_ROUND(SHA1_PARITY, 0);
    SHA1_ROUND(SHA1_PARITY, 1);
    SHA1_ROUND(SHA1_PARITY, 2);
    SHA1_ROUND(SHA1_PARITY, 3);

    return wasm_u32x4_make(a, b, c, d);
}

uint32x4_t vsha1mq_u32(uint32x4_t hash_abcd, uint32_t hash_e, uint32x4_t wk) {
    uint32_t a = wasm_u32x4_extract_lane(hash_abcd, 0);
    uint32_t b = wasm_u32x4_extract_lane(hash_abcd, 1);
    uint32_t c = wasm_u32x4_extract_lane(hash_abcd, 2);
    uint32_t d = wasm_u32x4_extract_lane(hash_abcd, 3);
    uint32_t e = hash_e;

    SHA1_ROUND(SHA1_MAJORITY, 0);
    SHA1_ROUND(SHA1_MAJORITY, 1);
    SHA1_ROUND(SHA1_MAJORITY, 2);
    SHA1_ROUND(SHA1_MAJORITY, 3);

    return wasm_u32x4_make(a, b, c, d);
}

uint32_t vsha1h_u32(uint32_t hash_e) {
    return __builtin_rotateleft32(hash_e, 30);
}

uint32x4_t vsha1su0q_u32(uint32x4_t w0_3, uint32x4_t w4_7, uint32x4_t w8_11) {
    v128_t operand1 = w0_3;
    v128_t operand2 = w4_7;
    v128_t operand3 = w8_11;

    // result = operand2<63:0> : operand1<127:64>;
    v128_t result =
        wasm_i64x2_make(wasm_i64x2_extract_lane(operand2, 0), wasm_i64x2_extract_lane(operand1, 1));

    // result = result EOR operand1 EOR operand3;
    result ^= operand1 ^ operand3;

    return result;
}

// TODO: uint32x4_t vld1q_u32(uint32_t const * ptr);
// TODO: void vst1q_u32(uint32_t * ptr, uint32x4_t val);

// TODO: uint32x4_t vsha1su1q_u32(uint32x4_t tw0_3, uint32x4_t w12_15);
