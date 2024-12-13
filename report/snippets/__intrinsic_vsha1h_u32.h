uint32x4_t __intrinsic_vsha1h_u32(uint32x4_t hash_e);

static inline uint32_t vsha1h_u32(uint32_t hash_e) {
    return wasm_u32x4_extract_lane(__intrinsic_vsha1h_u32(wasm_u32x4_splat(hash_e)), 0);
}
