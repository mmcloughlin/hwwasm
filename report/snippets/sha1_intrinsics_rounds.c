        // Rounds 28-31
        e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
        abcd = vsha1pq_u32(abcd, e1, t1);
        t1 = vaddq_u32(m1, vdupq_n_u32(K1));
        m2 = vsha1su1q_u32(m2, m1);
        m3 = vsha1su0q_u32(m3, m0, m1);
