#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

int matches(const char *name, const char *pattern) {
    while (*name && *pattern) {
        if (*pattern != '?' && *pattern != *name) {
            if (*pattern != '*') {
                return 0;
            }
            const char *p = pattern + 1;
            while (*p == '*') {
                p++;
            }
            if (*p == '\0') {
                return 1;
            }
            while (*name) {
                if (matches(name++, p)) {
                    return 1;
                }
            }
            return 0;
        }
        name++;
        pattern++;
    }
    return *name == '\0' && *pattern == '\0';
}

int isDirectory(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    return S_ISDIR(path_stat.st_mode);
}

void searchFiles(const char *path, const char *pattern) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

            if (matches(fullpath, pattern)) {
                printf("%s\n", fullpath);
            }

            if (isDirectory(fullpath) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                searchFiles(fullpath, pattern);
            }
        }
        closedir(dir);
    }
}

int main() {
    char pattern[100];

    printf("Enter pattern: ");
    if (fgets(pattern, sizeof(pattern), stdin) == NULL) {
        printf("Error reading pattern\n");
        return 1;
    }

    size_t len = strlen(pattern);
    if (len > 0 && pattern[len - 1] == '\n') {
        pattern[len - 1] = '\0';
    }

    searchFiles(".", pattern);

    return 0;
}
