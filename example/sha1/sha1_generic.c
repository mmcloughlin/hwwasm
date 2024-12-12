// SHA-1 implementation in plain C.

#include "sha1.h"

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

#define SHA1_LOAD_BE32(X)                                                            \
    ((uint32_t)((X)[0]) << 24 | (uint32_t)((X)[1]) << 16 | (uint32_t)((X)[2]) << 8 | \
     (uint32_t)((X)[3]))

#define SHA1_ROTL(X, N) __builtin_rotateleft32(X, N)

#define SHA1_CHOOSE(X, Y, Z) (((Y ^ Z) & X) ^ Z)

#define SHA1_PARITY(X, Y, Z) (X ^ Y ^ Z)

#define SHA1_MAJORITY(X, Y, Z) ((X & Y) | ((X | Y) & Z))

#define SHA1_WORD(I) W[(I) % 16]

#define SHA1_MESSAGE_SCHEDULE(I) \
    SHA1_WORD(I) =               \
        SHA1_ROTL(SHA1_WORD(I - 3) ^ SHA1_WORD(I - 8) ^ SHA1_WORD(I - 14) ^ SHA1_WORD(I - 16), 1);

#define SHA1_ROUND0(I, A, B, C, D, E, K, F)               \
    E += SHA1_ROTL(A, 5) + F(B, C, D) + K + SHA1_WORD(I); \
    B = SHA1_ROTL(B, 30);

#define SHA1_ROUND(I, A, B, C, D, E, K, F) \
    SHA1_MESSAGE_SCHEDULE(I)               \
    SHA1_ROUND0(I, A, B, C, D, E, K, F)

void sha1_blocks(uint32_t state[5], const uint8_t *data, size_t size) {
    uint32_t W[16];

    while (size >= SHA1_BLOCK_SIZE) {
        // Load state into working variables.
        uint32_t a = state[0];
        uint32_t b = state[1];
        uint32_t c = state[2];
        uint32_t d = state[3];
        uint32_t e = state[4];

        // Load message
        for (size_t i = 0; i < 16; i++) {
            W[i] = SHA1_LOAD_BE32(data);
            data += 4;
        }
        size -= SHA1_BLOCK_SIZE;

        // Rounds
        SHA1_ROUND0(0, a, b, c, d, e, K0, SHA1_CHOOSE);
        SHA1_ROUND0(1, e, a, b, c, d, K0, SHA1_CHOOSE);
        SHA1_ROUND0(2, d, e, a, b, c, K0, SHA1_CHOOSE);
        SHA1_ROUND0(3, c, d, e, a, b, K0, SHA1_CHOOSE);
        SHA1_ROUND0(4, b, c, d, e, a, K0, SHA1_CHOOSE);
        SHA1_ROUND0(5, a, b, c, d, e, K0, SHA1_CHOOSE);
        SHA1_ROUND0(6, e, a, b, c, d, K0, SHA1_CHOOSE);
        SHA1_ROUND0(7, d, e, a, b, c, K0, SHA1_CHOOSE);
        SHA1_ROUND0(8, c, d, e, a, b, K0, SHA1_CHOOSE);
        SHA1_ROUND0(9, b, c, d, e, a, K0, SHA1_CHOOSE);
        SHA1_ROUND0(10, a, b, c, d, e, K0, SHA1_CHOOSE);
        SHA1_ROUND0(11, e, a, b, c, d, K0, SHA1_CHOOSE);
        SHA1_ROUND0(12, d, e, a, b, c, K0, SHA1_CHOOSE);
        SHA1_ROUND0(13, c, d, e, a, b, K0, SHA1_CHOOSE);
        SHA1_ROUND0(14, b, c, d, e, a, K0, SHA1_CHOOSE);
        SHA1_ROUND0(15, a, b, c, d, e, K0, SHA1_CHOOSE);
        SHA1_ROUND(16, e, a, b, c, d, K0, SHA1_CHOOSE);
        SHA1_ROUND(17, d, e, a, b, c, K0, SHA1_CHOOSE);
        SHA1_ROUND(18, c, d, e, a, b, K0, SHA1_CHOOSE);
        SHA1_ROUND(19, b, c, d, e, a, K0, SHA1_CHOOSE);
        SHA1_ROUND(20, a, b, c, d, e, K1, SHA1_PARITY);
        SHA1_ROUND(21, e, a, b, c, d, K1, SHA1_PARITY);
        SHA1_ROUND(22, d, e, a, b, c, K1, SHA1_PARITY);
        SHA1_ROUND(23, c, d, e, a, b, K1, SHA1_PARITY);
        SHA1_ROUND(24, b, c, d, e, a, K1, SHA1_PARITY);
        SHA1_ROUND(25, a, b, c, d, e, K1, SHA1_PARITY);
        SHA1_ROUND(26, e, a, b, c, d, K1, SHA1_PARITY);
        SHA1_ROUND(27, d, e, a, b, c, K1, SHA1_PARITY);
        SHA1_ROUND(28, c, d, e, a, b, K1, SHA1_PARITY);
        SHA1_ROUND(29, b, c, d, e, a, K1, SHA1_PARITY);
        SHA1_ROUND(30, a, b, c, d, e, K1, SHA1_PARITY);
        SHA1_ROUND(31, e, a, b, c, d, K1, SHA1_PARITY);
        SHA1_ROUND(32, d, e, a, b, c, K1, SHA1_PARITY);
        SHA1_ROUND(33, c, d, e, a, b, K1, SHA1_PARITY);
        SHA1_ROUND(34, b, c, d, e, a, K1, SHA1_PARITY);
        SHA1_ROUND(35, a, b, c, d, e, K1, SHA1_PARITY);
        SHA1_ROUND(36, e, a, b, c, d, K1, SHA1_PARITY);
        SHA1_ROUND(37, d, e, a, b, c, K1, SHA1_PARITY);
        SHA1_ROUND(38, c, d, e, a, b, K1, SHA1_PARITY);
        SHA1_ROUND(39, b, c, d, e, a, K1, SHA1_PARITY);
        SHA1_ROUND(40, a, b, c, d, e, K2, SHA1_MAJORITY);
        SHA1_ROUND(41, e, a, b, c, d, K2, SHA1_MAJORITY);
        SHA1_ROUND(42, d, e, a, b, c, K2, SHA1_MAJORITY);
        SHA1_ROUND(43, c, d, e, a, b, K2, SHA1_MAJORITY);
        SHA1_ROUND(44, b, c, d, e, a, K2, SHA1_MAJORITY);
        SHA1_ROUND(45, a, b, c, d, e, K2, SHA1_MAJORITY);
        SHA1_ROUND(46, e, a, b, c, d, K2, SHA1_MAJORITY);
        SHA1_ROUND(47, d, e, a, b, c, K2, SHA1_MAJORITY);
        SHA1_ROUND(48, c, d, e, a, b, K2, SHA1_MAJORITY);
        SHA1_ROUND(49, b, c, d, e, a, K2, SHA1_MAJORITY);
        SHA1_ROUND(50, a, b, c, d, e, K2, SHA1_MAJORITY);
        SHA1_ROUND(51, e, a, b, c, d, K2, SHA1_MAJORITY);
        SHA1_ROUND(52, d, e, a, b, c, K2, SHA1_MAJORITY);
        SHA1_ROUND(53, c, d, e, a, b, K2, SHA1_MAJORITY);
        SHA1_ROUND(54, b, c, d, e, a, K2, SHA1_MAJORITY);
        SHA1_ROUND(55, a, b, c, d, e, K2, SHA1_MAJORITY);
        SHA1_ROUND(56, e, a, b, c, d, K2, SHA1_MAJORITY);
        SHA1_ROUND(57, d, e, a, b, c, K2, SHA1_MAJORITY);
        SHA1_ROUND(58, c, d, e, a, b, K2, SHA1_MAJORITY);
        SHA1_ROUND(59, b, c, d, e, a, K2, SHA1_MAJORITY);
        SHA1_ROUND(60, a, b, c, d, e, K3, SHA1_PARITY);
        SHA1_ROUND(61, e, a, b, c, d, K3, SHA1_PARITY);
        SHA1_ROUND(62, d, e, a, b, c, K3, SHA1_PARITY);
        SHA1_ROUND(63, c, d, e, a, b, K3, SHA1_PARITY);
        SHA1_ROUND(64, b, c, d, e, a, K3, SHA1_PARITY);
        SHA1_ROUND(65, a, b, c, d, e, K3, SHA1_PARITY);
        SHA1_ROUND(66, e, a, b, c, d, K3, SHA1_PARITY);
        SHA1_ROUND(67, d, e, a, b, c, K3, SHA1_PARITY);
        SHA1_ROUND(68, c, d, e, a, b, K3, SHA1_PARITY);
        SHA1_ROUND(69, b, c, d, e, a, K3, SHA1_PARITY);
        SHA1_ROUND(70, a, b, c, d, e, K3, SHA1_PARITY);
        SHA1_ROUND(71, e, a, b, c, d, K3, SHA1_PARITY);
        SHA1_ROUND(72, d, e, a, b, c, K3, SHA1_PARITY);
        SHA1_ROUND(73, c, d, e, a, b, K3, SHA1_PARITY);
        SHA1_ROUND(74, b, c, d, e, a, K3, SHA1_PARITY);
        SHA1_ROUND(75, a, b, c, d, e, K3, SHA1_PARITY);
        SHA1_ROUND(76, e, a, b, c, d, K3, SHA1_PARITY);
        SHA1_ROUND(77, d, e, a, b, c, K3, SHA1_PARITY);
        SHA1_ROUND(78, c, d, e, a, b, K3, SHA1_PARITY);
        SHA1_ROUND(79, b, c, d, e, a, K3, SHA1_PARITY);

        // Combine state
        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
        state[4] += e;
    }
}
