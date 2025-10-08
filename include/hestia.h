/*
 * Copyright (c) 2025 Théo Garcia
 * Licensed under the MIT License
 */

#ifndef HESTIA_HESTIA_H
#define HESTIA_HESTIA_H

#include <stdbool.h>

typedef struct file_paths_s {
    char *home_path;
    char *hestia_folder_path;
} file_paths_t;

int connection(void);
int create_password(void);

#endif //HESTIA_HESTIA_H
