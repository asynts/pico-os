/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * Copyright (c) 2022 Paul Scharnofske
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <assert.h>
#include <stdio.h>

uint32_t crc32_small(const uint8_t *buf, unsigned int len, uint32_t seed);

#define BOOT2_SIZE_BYTES 256

int main(int argc, char **argv) {
    // The boot sector is passed via stdin.
    uint8_t boot2_load[BOOT2_SIZE_BYTES];
    int retval = fread(boot2_load, 1, BOOT2_SIZE_BYTES, stdin);
    assert(retval == BOOT2_SIZE_BYTES);

    uint32_t sum = crc32_small(boot2_load, BOOT2_SIZE_BYTES - 4, 0xffffffff);

    if (sum == *(uint32_t *) (boot2_load + BOOT2_SIZE_BYTES - 4)) {
        printf("passed\n");
    } else {
        printf("failed, expected: %#x\n", sum);
    }
}
