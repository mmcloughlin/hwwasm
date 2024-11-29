#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sha1.h"

#define MESSAGE_BLOCKS (UINT64_C(1) << 6)
#define MESSAGE_SIZE (MESSAGE_BLOCKS * SHA1_BLOCK_SIZE)
#define ITERATIONS (UINT64_C(1) << 17)
#define TOTAL_BLOCKS (MESSAGE_BLOCKS * ITERATIONS)

static uint64_t nanotime() {
    struct timespec ts;
    const int status = clock_gettime(CLOCK_MONOTONIC, &ts);
    if (status != 0) {
        abort();
    }
    return (uint64_t)ts.tv_sec * UINT64_C(1000000000) + ts.tv_nsec;
}

int main() {
    // Empty.
    uint8_t message[MESSAGE_SIZE] = {0};
    for (size_t i = 0; i < MESSAGE_SIZE; i++) {
        message[i] = (uint8_t)i;
    }

    // Benchmark.
    uint32_t state[5];
    sha1_state_init(state);
    const uint64_t start = nanotime();
    for (size_t i = 0; i < ITERATIONS; i++) {
        sha1_blocks(state, message, sizeof(message));
    }
    const uint64_t end = nanotime();
    const uint64_t elapsed_ns = end - start;

    // Report.
    printf("{\n");

    // Parameters.
    printf("  \"message_blocks\": %" PRIu64 ",\n", MESSAGE_BLOCKS);
    printf("  \"iterations\": %" PRIu64 ",\n", ITERATIONS);
    printf("  \"total_blocks\": %" PRIu64 ",\n", TOTAL_BLOCKS);

    // State: include for comparison and to prevent dead code elimination.
    printf("  \"final_state\":");
    for (size_t i = 0; i < 5; i++) {
        char *sep = i == 0 ? " [" : ", ";
        printf("%s\"%08" PRIx32 "\"", sep, state[i]);
    }
    printf("],\n");

    // Timing.
    printf("  \"elapsed_ns\": %" PRIu64 "\n", elapsed_ns);

    printf("}\n");

    return EXIT_SUCCESS;
}
