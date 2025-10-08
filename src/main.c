/*
 * Copyright (c) 2025 Théo Garcia
 * Licensed under the MIT License
 */

#include "hestia.h"
#include <stdlib.h>
#include <limits.h>
#include <sodium.h>

int main(void)
{
    if (sodium_init() < 0) {
        fprintf(stderr, "Libsodium init failed.\n");
        return EXIT_FAILURE;
    }
    if (connection() == -1)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}