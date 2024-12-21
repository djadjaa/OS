#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define check_error(expr, userMsg)\
    do {\
        if(!(expr)) {\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    } while (0)

#define MAX_SIZE 256

int main(int argc, char** argv) {
    check_error(argc == 2, "argc");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");

    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "fopen");

    //char buffer[MAX_SIZE + 1];

    struct flock lock;

    //int len = strlen(buffer);

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_CUR;
    lock.l_start = 1;
    lock.l_len = 5;

    check_error(fcntl(fd, F_SETLK, &lock) != -1, "asdasdasdasd");

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_CUR;
    lock.l_start = 15;
    lock.l_len = 6;

    check_error(fcntl(fd, F_SETLK, &lock) != -1, "asdasdasdasd");

    sleep(10);

    fclose(f);
    exit(EXIT_SUCCESS);
}