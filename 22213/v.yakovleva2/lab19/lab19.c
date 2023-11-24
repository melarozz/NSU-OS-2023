#include <stdio.h>
#include <dirent.h>
#include <string.h>

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
int main() {
    char pattern[100];
    struct dirent *entry;
    DIR *dir;
    int found = 0;

    if ((dir = opendir(".")) == NULL) {
        perror("Couldn't open current directory");
        return 1;
    }

    printf("Enter pattern: ");
    scanf("%s", pattern);

    if (strchr(pattern, '/') != NULL) {
        printf("Using / is restricted\n");
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (matches(entry->d_name, pattern)) {
            printf("%s\n", entry->d_name);
            found = 1;
        }
    }

    if (!found) {
        printf("%s did not match any file in the current directory\n", pattern);
    }

    closedir(dir);
    return 0;
}