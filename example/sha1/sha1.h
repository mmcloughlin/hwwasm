#pragma once

#include <stddef.h>
#include <stdint.h>

// SHA-1 block size in bytes.
#define SHA1_BLOCK_SIZE 64

// SHA-1 hash multiple blocks of data.
void sha1_blocks(uint32_t state[5], const uint8_t *data, size_t size);
