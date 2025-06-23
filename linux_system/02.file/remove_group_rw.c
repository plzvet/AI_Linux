#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main() {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    mode_t new_mode;

    dir = opendir(".");
    if (!dir) {
        perror("opendir");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL) {
        // 1. 숨김 파일, . .. 제거
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // 2. 이름이 "file"로 시작하지 않으면 스킵
        if (strncmp(entry->d_name, "file", 4) != 0)
            continue;

        // 3. 파일 상태 확인
        if (stat(entry->d_name, &st) == -1) {
            perror("stat");
            continue;
        }

        // 4. 일반 파일만 처리
        if (!S_ISREG(st.st_mode))
            continue;

        // 5. group rw- 권한 확인 및 제거
        if ((st.st_mode & (S_IRGRP | S_IWGRP)) == (S_IRGRP | S_IWGRP)) {
            printf("Removing group rw- from: %s\n", entry->d_name);
            new_mode = st.st_mode & ~(S_IRGRP | S_IWGRP);

            if (chmod(entry->d_name, new_mode) == -1) {
                perror("chmod");
            }
        }
    }

    closedir(dir);
    return 0;
}

