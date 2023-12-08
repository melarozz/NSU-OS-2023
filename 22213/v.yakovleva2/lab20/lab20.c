#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#define CHUNK_SIZE 10

int patternFound = 0;

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

    if ((dir = opendir(path)) == NULL) {
        perror("opendir() error");
        return 1;
    } else {
        while ((entry = readdir(dir)) != NULL) {
            size_t path_len = strlen(path);
            size_t entry_name_len = strlen(entry->d_name);
            size_t fullpath_len = path_len + entry_name_len + 2; // +1 for '/' and +1 for null terminator

            char *fullpath = (char *)malloc(fullpath_len);
            if (fullpath == NULL) {
                printf("Memory allocation error.\n");
                closedir(dir);
                return;
            }
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

            if (matches(fullpath, pattern)) {
                printf("%s\n", fullpath);
                patternFound = 1;
            }

            if (isDirectory(fullpath) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                searchFiles(fullpath, pattern);
            }
        }
        closedir(dir);
    }
}

int main() {
    size_t currentSize = CHUNK_SIZE;
    size_t usedSize = 0;
    char *pattern = (char *)malloc(currentSize * sizeof(char));

    if (pattern == NULL) {
        printf("Memory allocation error.\n");
        return 1;
    }

    printf("Enter pattern: ");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (usedSize + 1 >= currentSize) {
            currentSize += CHUNK_SIZE;
            char *temp = (char *)realloc(pattern, currentSize * sizeof(char));
            if (temp == NULL) {
                printf("Memory reallocation error.\n");
                free(pattern);
                return 1;
            }
            pattern = temp;
        }
        pattern[usedSize++] = (char)c;
    }
    pattern[usedSize] = '\0';

    searchFiles(".", pattern);

    if (!patternFound) {
        printf("No files matching the pattern '%s' found. \n", pattern);
    }

    free(pattern);
    return 0;
}

