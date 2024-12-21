#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);


int main(int argc,char **argv){

    checkError(argc==4,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISREG(fInfo.st_mode),"not regular");
    int fd = open(argv[1],O_RDONLY);
    checkError(fd!=-1,"open");
    int offset = strtol(argv[2],NULL,10);
    int lenght = strtol(argv[3],NULL,10);
    checkError(lseek(fd,offset,SEEK_SET)!=-1,"lseek"); //od pocetka citamo sve do offseta
    char *buff = malloc(lenght+1);
    checkError(buff!=NULL,"malloc");

    int readBytes=0;
    while(lenght>0){
        checkError((readBytes=read(fd,buff,lenght))!=-1,"read");
        checkError(write(STDOUT_FILENO,buff,readBytes)!=-1,"write");
        lenght-=readBytes;
    }

    free(buff);
    close(fd);

    exit(EXIT_SUCCESS);
}