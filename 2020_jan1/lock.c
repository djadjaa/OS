#define _XOPEN_SOURCE 700

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <ctype.h>

#define check_error(expr, userMsg)\
    do {\
        if(!(expr)) {\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    } while (0)

int main(int argc, char** argv) {

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");

    struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 9;

    int ret = fcntl(fd, F_SETLK, &lock);
    printf("%d\n", ret);

            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 36;
            lock.l_len = 2;

    ret = fcntl(fd, F_SETLK, &lock);

    printf("%d\n", ret);

    lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 20;
            lock.l_len = 3;

    ret = fcntl(fd, F_SETLK, &lock);

    printf("%d\n", ret);

    sleep(20);

    exit(EXIT_SUCCESS);
}