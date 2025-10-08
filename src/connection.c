/*
 * Copyright (c) 2025 Théo Garcia
 * Licensed under the MIT License
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "return_code.h"
#include "hestia.h"

static char *check_if_home_exist(void)
{
    char *home = getenv("HOME");

    if (!home) {
        fprintf(stderr, "getenv: couldn't find HOME environment variable\n");
        return NULL;
    }
    return home;
}

static int check_if_hestia_folder_exist(file_paths_t paths[static 1])
{
    DIR *hestia_dir;

    paths->home_path = check_if_home_exist();
    if (!paths->home_path)
        return -1;
    if (asprintf(&paths->hestia_folder_path, "%s/.hestia", paths->home_path) == -1) {
        fprintf(stderr, "asprintf: fail\n");
        return SYSCALL_ERROR;
    }
    hestia_dir = opendir(paths->hestia_folder_path);
    if (hestia_dir)
        return 0;
    if (errno == ENOENT)
        return 1;
    perror("opendir");
    return SYSCALL_ERROR;
}

static bool create_hestia_folder(file_paths_t paths[static 1])
{
    if (mkdir(paths->hestia_folder_path, 0700) == -1) {
        perror("mkdir");
        return false;
    }
    printf("Created .hestia folder in home\n");
    return true;
}

void free_file_paths(file_paths_t *paths)
{
    free(paths->hestia_folder_path);
    free(paths);
}

int check_if_directory_empty(char *hestia_folder_path)
{
    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(hestia_folder_path);

    if (dir == NULL) {
        perror("opendir");
        return SYSCALL_ERROR;
    }
    while ((d = readdir(dir))) {
        if (d == NULL) {
            perror("readdir");
            return SYSCALL_ERROR;
        }
        if(++n > 2)
            break;
    }
    closedir(dir);
    if (n <= 2)
        return 0;
    else
        return 1;
}

int print_welcome(char *hestia_folder_path)
{
    int result;

    printf("Welcome to hestia, a lightweight CLI password manager.\n");
    result = check_if_directory_empty(hestia_folder_path);
    if (result == SYSCALL_ERROR)
        return SYSCALL_ERROR;
    if (result == 0)
        create_password();
    return 0;
}

int connection(void)
{
    int result = 0;
    file_paths_t *paths = malloc(sizeof(file_paths_t));

    if (!paths) {
        fprintf(stderr, "malloc: not enough memory available\n");
        exit(EXIT_FAILURE);
    }
    result = check_if_hestia_folder_exist(paths);
    if (result == SYSCALL_ERROR) {
        free_file_paths(paths);
        exit(EXIT_FAILURE);
    } else if (result == 1) {
        if (!create_hestia_folder(paths)) {
            free_file_paths(paths);
            return SYSCALL_ERROR;
        }
    }
    print_welcome(paths->hestia_folder_path);
    free_file_paths(paths);
    return 0;
}
