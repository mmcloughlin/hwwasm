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
