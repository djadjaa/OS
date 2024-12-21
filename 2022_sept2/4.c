#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 2
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

    checkError(argc==2,"args");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"Open");
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");
    char c;
    int offset=0;
    int numLocked=0;
    while(read(fd,&c,1)>0){
        struct flock lock;
        lock.l_type=F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = offset;
        lock.l_len = 1; //karakter po karakter

        checkError(fcntl(fd,F_GETLK,&lock)!=-1,"fcntl");

        if(lock.l_type!=F_UNLCK)
            numLocked++;
        offset++;
    }
    printf("%d\n",numLocked);
    fclose(f);
    exit(EXIT_SUCCESS);
}
