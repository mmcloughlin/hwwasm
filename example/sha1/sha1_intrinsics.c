// SHA-1 implementation using ARM intrinsics.
//
// Adapted from the public domain implementation:
//  https://github.com/noloader/SHA-Intrinsics/blob/4899efc81d1af159c1fd955936c673139f35aea9/sha1-arm.c

#include "sha1.h"

#include "intrinsics.h"

// Round constants
#define K0 0x5a827999
#define K1 0x6ed9eba1
#define K2 0x8f1bbcdc
#define K3 0xca62c1d6

void sha1_state_init(uint32_t state[5]) {
    state[0] = 0x67452301;
    state[1] = 0xefcdab89;
    state[2] = 0x98badcfe;
    state[3] = 0x10325476;
    state[4] = 0xc3d2e1f0;
}

void sha1_blocks(uint32_t state[5], const uint8_t *data, size_t size) {
    uint32x4_t abcd, abcd_saved;
    uint32x4_t t0, t1;
    uint32x4_t m0, m1, m2, m3;
    uint32_t e0, e0_saved, e1;

    // Load state
    abcd = vld1q_u32(&state[0]);
    e0 = state[4];

    while (size >= SHA1_BLOCK_SIZE) {
        // Save state
        abcd_saved = abcd;
        e0_saved = e0;

        // Load message
        m0 = vld1q_u32((const uint32_t *)(data));
        m1 = vld1q_u32((const uint32_t *)(data + 16));
        m2 = vld1q_u32((const uint32_t *)(data + 32));
        m3 = vld1q_u32((const uint32_t *)(data + 48));

        // Reverse for little endian
        m0 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(m0)));
        m1 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(m1)));
        m2 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(m2)));
        m3 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(m3)));

        t0 = vaddq_u32(m0, vdupq_n_u32(K0));
        t1 = vaddq_u32(m1, vdupq_n_u32(K0));

        // Rounds 0-3
        e1 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1cq_u32(abcd, e0, t0);
        t0 = vaddq_u32(m2, vdupq_n_u32(K0));
        m0 = vsha1su0q_u32(m0, m1, m2);

        // Rounds 4-7
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1cq_u32(abcd, e1, t1);
        t1 = vaddq_u32(m3, vdupq_n_u32(K0));
        m0 = vsha1su1q_u32(m0, m3);
        m1 = vsha1su0q_u32(m1, m2, m3);

        // Rounds 8-11
        e1 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1cq_u32(abcd, e0, t0);
        t0 = vaddq_u32(m0, vdupq_n_u32(K0));
        m1 = vsha1su1q_u32(m1, m0);
        m2 = vsha1su0q_u32(m2, m3, m0);

        // Rounds 12-15
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1cq_u32(abcd, e1, t1);
        t1 = vaddq_u32(m1, vdupq_n_u32(K1));
        m2 = vsha1su1q_u32(m2, m1);
        m3 = vsha1su0q_u32(m3, m0, m1);

        // Rounds 16-19
        e1 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1cq_u32(abcd, e0, t0);
        t0 = vaddq_u32(m2, vdupq_n_u32(K1));
        m3 = vsha1su1q_u32(m3, m2);
        m0 = vsha1su0q_u32(m0, m1, m2);

        // Rounds 20-23
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e1, t1);
        t1 = vaddq_u32(m3, vdupq_n_u32(K1));
        m0 = vsha1su1q_u32(m0, m3);
        m1 = vsha1su0q_u32(m1, m2, m3);

        // Rounds 24-27
        e1 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e0, t0);
        t0 = vaddq_u32(m0, vdupq_n_u32(K1));
        m1 = vsha1su1q_u32(m1, m0);
        m2 = vsha1su0q_u32(m2, m3, m0);

        // Rounds 28-31
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e1, t1);
        t1 = vaddq_u32(m1, vdupq_n_u32(K1));
        m2 = vsha1su1q_u32(m2, m1);
        m3 = vsha1su0q_u32(m3, m0, m1);

        // Rounds 32-35
        e1 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e0, t0);
        t0 = vaddq_u32(m2, vdupq_n_u32(K2));
        m3 = vsha1su1q_u32(m3, m2);
        m0 = vsha1su0q_u32(m0, m1, m2);

        // Rounds 36-39
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e1, t1);
        t1 = vaddq_u32(m3, vdupq_n_u32(K2));
        m0 = vsha1su1q_u32(m0, m3);
        m1 = vsha1su0q_u32(m1, m2, m3);

        // Rounds 40-43
        e1 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1mq_u32(abcd, e0, t0);
        t0 = vaddq_u32(m0, vdupq_n_u32(K2));
        m1 = vsha1su1q_u32(m1, m0);
        m2 = vsha1su0q_u32(m2, m3, m0);

        // Rounds 44-47
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1mq_u32(abcd, e1, t1);
        t1 = vaddq_u32(m1, vdupq_n_u32(K2));
        m2 = vsha1su1q_u32(m2, m1);
        m3 = vsha1su0q_u32(m3, m0, m1);

        // Rounds 48-51
        e1 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1mq_u32(abcd, e0, t0);
        t0 = vaddq_u32(m2, vdupq_n_u32(K2));
        m3 = vsha1su1q_u32(m3, m2);
        m0 = vsha1su0q_u32(m0, m1, m2);

        // Rounds 52-55
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1mq_u32(abcd, e1, t1);
        t1 = vaddq_u32(m3, vdupq_n_u32(K3));
        m0 = vsha1su1q_u32(m0, m3);
        m1 = vsha1su0q_u32(m1, m2, m3);

        // Rounds 56-59
        e1 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1mq_u32(abcd, e0, t0);
        t0 = vaddq_u32(m0, vdupq_n_u32(K3));
        m1 = vsha1su1q_u32(m1, m0);
        m2 = vsha1su0q_u32(m2, m3, m0);

        // Rounds 60-63
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e1, t1);
        t1 = vaddq_u32(m1, vdupq_n_u32(K3));
        m2 = vsha1su1q_u32(m2, m1);
        m3 = vsha1su0q_u32(m3, m0, m1);

        // Rounds 64-67
        e1 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e0, t0);
        t0 = vaddq_u32(m2, vdupq_n_u32(K3));
        m3 = vsha1su1q_u32(m3, m2);
        m0 = vsha1su0q_u32(m0, m1, m2);

        // Rounds 68-71
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e1, t1);
        t1 = vaddq_u32(m3, vdupq_n_u32(K3));
        m0 = vsha1su1q_u32(m0, m3);

        // Rounds 72-75
        e1 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e0, t0);

        // Rounds 76-79
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e1, t1);

        // Combine state
        e0 += e0_saved;
        abcd = vaddq_u32(abcd_saved, abcd);

        data += SHA1_BLOCK_SIZE;
        size -= SHA1_BLOCK_SIZE;
    }

    // Save state
    vst1q_u32(&state[0], abcd);
    state[4] = e0;
}
