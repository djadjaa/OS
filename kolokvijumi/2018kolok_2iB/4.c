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

    checkError(argc==4,"args");
    int fd = open(argv[1],O_RDONLY);
    checkError(fd!=-1,"open");
    int offset = atoi(argv[2]);
    int size = atoi(argv[3]);

    checkError(lseek(fd,offset,SEEK_SET)!=-1,"lseek");

    char *buff = malloc(size+1);
    checkError(buff!=NULL,"malloc");

    int readBytes=0;
    while(size>0){
        checkError((readBytes=read(fd,buff,size))!=-1,"read");
        checkError(write(STDOUT_FILENO,buff,size)!=-1,"write");
        size -= readBytes;
    }

    free(buff);
    close(fd);
    exit(EXIT_SUCCESS);
}