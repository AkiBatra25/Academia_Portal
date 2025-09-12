// server/common.c
#include "../include/common.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include<stdbool.h>

bool authenticate(const char *username, const char *password, const char *expected_role, struct User *out_user) {
    FILE *fp = fopen("server/users.dat", "rb");
    if (!fp) return false;

    struct User u;
    while (fread(&u, sizeof(u), 1, fp)) {
        if (strcmp(u.username, username) == 0 &&
            strcmp(u.password, password) == 0 &&
            strcmp(u.role, expected_role) == 0 &&
            u.active == 1) {
            *out_user = u;
            fclose(fp);
            return true;
        }
    }

    fclose(fp);
    return false;
}


void lock_file(int fd, short type) {
    struct flock fl = {0};
    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fcntl(fd, F_SETLKW, &fl);
}

void unlock_file(int fd) {
    struct flock fl = {0};
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fcntl(fd, F_SETLK, &fl);
}
