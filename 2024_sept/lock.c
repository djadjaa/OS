#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 256

int main(int argc, char **argv){

    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"");
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"");
    char buff[MAX];
    struct flock lock;
    int len = strlen(buff);
    /*
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_CUR;
    lock.l_start = 16;
    lock.l_len = 7;

    checkError(fcntl(fd,F_SETLK,&lock)!=-1,"");

    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_CUR;
    lock.l_start = 5;
    lock.l_len = 5;*/
    
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_CUR;
    lock.l_start = 0;
    lock.l_len = 10000;
    checkError(fcntl(fd,F_SETLK,&lock)!=-1,"");

    sleep(20);

    fclose(f);
    exit(EXIT_SUCCESS);
}
