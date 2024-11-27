#include <stdlib.h>
#include <string.h>

#include "sha1.h"

int main() {
    // Empty message and expected hash.
    uint8_t message[SHA1_BLOCK_SIZE] = {0x80};
    uint32_t expect[5] = {0xda39a3ee, 0x5e6b4b0d, 0x3255bfef, 0x95601890, 0xafd80709};

    // Hash.
    uint32_t state[5];
    sha1_state_init(state);
    sha1_blocks(state, message, sizeof(message));

    // Check.
    if (0 != memcmp(state, expect, sizeof(expect))) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
