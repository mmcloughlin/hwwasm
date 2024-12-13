uint32x4_t vld1q_u32(uint32_t const *ptr);
void vst1q_u32(uint32_t *ptr, uint32x4_t val);
uint32x4_t vaddq_u32(uint32x4_t a, uint32x4_t b);
uint32x4_t vdupq_n_u32(uint32_t value);
uint8x16_t vrev32q_u8(uint8x16_t vec);
uint32_t vgetq_lane_u32(uint32x4_t v, const int lane);
uint32x4_t vreinterpretq_u32_u8(uint8x16_t a);
uint8x16_t vreinterpretq_u8_u32(uint32x4_t a);
