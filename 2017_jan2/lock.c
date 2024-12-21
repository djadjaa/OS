#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 256

int main(int argc, char **argv){

    checkError(argc==2,"");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"");

    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 7;
    lock.l_len = 3;
    fcntl(fd,F_SETLK,&lock);

    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 39;
    lock.l_len = 3;

    fcntl(fd,F_SETLK,&lock);

    sleep(15);

    close(fd);
    exit(EXIT_SUCCESS);
}