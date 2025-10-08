/*
 * Copyright (c) 2025 Théo Garcia
 * Licensed under the MIT License
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sodium.h>
#include "return_code.h"

#define HASH_LEN crypto_pwhash_STRBYTES
#define MAX_PASSWORD_LEN 256

int store_master_password(const char *password, char *out_hash) {
    if (crypto_pwhash_str(
        out_hash,
        password,
        strlen(password),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        return -1;
    }
    return 0;
}

int get_hidden_input(char *buffer, int size)
{
    struct termios oldt;
    struct termios newt;

    if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
        perror("tcgetattr");
        return SYSCALL_ERROR;
    }
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1) {
        perror("tcsetattr");
        return SYSCALL_ERROR;
    }
    if (fgets(buffer, size, stdin) == NULL)
        return 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) == -1) {
        perror("tcsetattr");
        return SYSCALL_ERROR;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

void memzero_master_and_copy(char *master_password, char *copy)
{
    sodium_memzero(master_password, sizeof(master_password));
    sodium_memzero(copy, sizeof(copy));
}

int create_password(void)
{
    char master_password[MAX_PASSWORD_LEN];
    char copy[MAX_PASSWORD_LEN];
    char hash[HASH_LEN];
    int tries = 3;

    printf("For your first connection, create your master password.\n");
    printf("This will be the only password you need to remember.\n\n");
    printf("Enter master password: ");
    fflush(stdout);
    get_hidden_input(master_password, sizeof(master_password));
    printf("\n\nEnter your password again: ");
    while (tries >= 0) {
        fflush(stdout);
        get_hidden_input(copy, sizeof(copy));
        if (strcmp(master_password, copy) == 0) {
            if (store_master_password(master_password, hash) != 0) {
                fprintf(stderr, "Error hashing password\n");
                return 1;
            }
            printf("\n\nPassword set successfully!\n");
            break;
        } else if (tries != 0){
            printf("\n\nPasswords don't match, try again (%d tries left): ", tries);
            tries--;
        }
    }
    memzero_master_and_copy(master_password, copy);
    if (tries == 0) {
        printf("\nToo many failed attempts. Aborting.\n");
        return 1;
    }
    return 0;
}
