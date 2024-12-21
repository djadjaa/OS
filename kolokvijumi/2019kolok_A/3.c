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

    checkError(argc==5,"args");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");
    int a = atoi(argv[2]);
    int b = atoi(argv[3]);
    int n = atoi(argv[4]);

    char *buff_a = malloc(n+1);
    checkError(buff_a!=NULL,"malloc");
    char *buff_b = malloc(n+1);
    checkError(buff_b!=NULL,"malloc");

    checkError(lseek(fd,a,SEEK_SET)!=-1,"lseek");
    int readBytes=0;
    checkError((readBytes=read(fd,buff_a,n))!=-1,"read");
    checkError(readBytes==n,"end with reading buff_a");

    checkError(lseek(fd,b,SEEK_SET)!=-1,"lseek");
    checkError((readBytes=read(fd,buff_b,n))!=-1,"read");
    checkError(readBytes==n,"end with reading buff_b");

    checkError(lseek(fd,a,SEEK_SET)!=-1,"lseek");
    int writeBytes=0;
    checkError((writeBytes=write(fd,buff_b,n))!=-1,"write");
    checkError(writeBytes==n,"end with writing buff_b");

    checkError(lseek(fd,b,SEEK_SET)!=-1,"LSEEK");
    checkError((writeBytes=write(fd,buff_a,n))!=-1,"write");
    checkError(writeBytes==n,"end with writing buff_a");

    free(buff_a);
    free(buff_b);
    close(fd);
    exit(EXIT_SUCCESS);
}