#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){

    checkError(argc==2,"");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"");

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_len = 8;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;

    checkError(fcntl(fd,F_SETLK,&lock)!=-1,"");

    lock.l_type = F_RDLCK;
    lock.l_len = 5;
    lock.l_start = 9;
    lock.l_whence = SEEK_SET;

    checkError(fcntl(fd,F_SETLK,&lock)!=-1,"");

    lock.l_type = F_WRLCK;
    lock.l_len = 11;
    lock.l_start = 2;
    lock.l_whence = SEEK_SET;

    checkError(fcntl(fd,F_SETLK,&lock)!=-1,"");

    lock.l_type = F_RDLCK;
    lock.l_len = 7;
    lock.l_whence = SEEK_SET;
    lock.l_start = 26;

    sleep(20);

    lock.l_type = F_UNLCK;
    checkError(fcntl(fd,F_SETLK,&lock)!=-1,"");

    exit(EXIT_SUCCESS);
}
