#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
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
    checkError(fd!=-1,"");
    /*
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"");*/
    int len =0;
    char c;
    while(read(fd,&c,1)==1){
        if(c==' ' || c=='\n')
            break;
        len++;
    }
    checkError(len>0,"");
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_len = len;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;

    checkError(fcntl(fd,F_SETLKW,&lock)!=-1,"");
    printf("zakljucano!");
    checkError(lseek(fd,0,SEEK_SET)!=-1,"");
    for(int i=0; i<len; i++)
        checkError(write(fd,"#",1)!=-1,"");

    lock.l_type =F_UNLCK;
    checkError(fcntl(fd,F_SETLK,&lock)!=-1,"");
    //fclose(f);
    close(fd);
    exit(EXIT_SUCCESS);
}
