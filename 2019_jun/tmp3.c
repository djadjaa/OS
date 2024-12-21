#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Funkcija koja proverava da li je linija u fajlu zaključana
int is_line_locked(int fd, off_t offset, size_t length) {
    struct flock fl;
    fl.l_type = F_WRLCK;  // Provera za zaključavanje za pisanje
    fl.l_whence = SEEK_SET;
    fl.l_start = offset;
    fl.l_len = length;
    fl.l_pid = 0;

    if (fcntl(fd, F_GETLK, &fl) == -1) {
        perror("fcntl");
        return -1;
    }

    // Ako je tip zaključavanja F_UNLCK, linija nije zaključana
    if (fl.l_type == F_UNLCK) {
        return 0;
    }

    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("fopen");
        return 1;
    }

    int fd = fileno(file);
    if (fd == -1) {
        perror("fileno");
        fclose(file);
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    off_t offset = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        if (is_line_locked(fd, offset, read) == 0) {
            printf("%s", line);
        }
        offset += read;
    }

    free(line);
    fclose(file);
    return 0;
}
