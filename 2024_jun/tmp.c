#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_DIR_LENGTH 1024

int can_read_dir(const char *dir_path) {
    struct stat dir_stat;
    if (stat(dir_path, &dir_stat) != 0) {
        return 0; // Stat nije uspeo, nema pristupa
    }

    // Provera prava čitanja za korisnika, grupu i ostale
    if ((dir_stat.st_mode & S_IRUSR) && // Pravo čitanja za korisnika
        (dir_stat.st_mode & S_IRGRP) && // Pravo čitanja za grupu
        (dir_stat.st_mode & S_IROTH)) { // Pravo čitanja za ostale
        return 1;
    }
    
    return 0;
}

int is_directory(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

int main() {
    const char *path_env = getenv("PATH");
    if (!path_env) {
        printf("PATH promenljiva nije definisana.\n");
        return 1;
    }

    // Kopiranje PATH promenljive za obradu
    char path_copy[MAX_DIR_LENGTH];
    strncpy(path_copy, path_env, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';
    printf("path: %s\n",path_copy);
    char *dir = strtok(path_copy, ":");
    while (dir != NULL) {
        if (is_directory(dir) && can_read_dir(dir)) {
            char *dir_name = strrchr(dir, '/');
            if (dir_name != NULL) {
                printf("%s\n", dir_name + 1); // Ispisujemo samo naziv direktorijuma
            } else {
                printf("%s\n", dir); // Ako nema '/' ispisujemo ceo direktorijum
            }
        }

        dir = strtok(NULL, ":");
    }

    return 0;
}
