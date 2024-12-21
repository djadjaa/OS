#define _XOPEN_SOURCE 500
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

#define MAX 8096
int main(int argc,char **argv){

    checkError(argc==3,"args");
    int inputFd, outputFd;
    if(strcmp(argv[1],"-r")==0){ //read
        //tada na standardni izlaz ispisujemo sadrzaj
        inputFd = open(argv[2],O_RDONLY);
        checkError(inputFd!=-1,"open");
        outputFd = STDOUT_FILENO;
    }
    else if(strcmp(argv[1],"-w")==0){ //write
        inputFd = STDIN_FILENO;
        outputFd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC);
        checkError(outputFd!=-1,"open");
    }
    else if(strcmp(argv[1],"-a")==0){ //append
        inputFd = STDIN_FILENO;
        outputFd = open(argv[2],O_WRONLY|O_APPEND|O_CREAT);
        checkError(outputFd!=-1,"open");
    }
    else{
        checkError(0,"wrong choice");
    }
    
    int readBytes = 0;
    char buff[MAX];
    while((readBytes=read(inputFd,buff,MAX))>0){
        checkError(write(outputFd,buff,readBytes)!=-1,"write");
    }
    checkError(readBytes!=-1,"read");

    //koji fd zatvaramo ?
    if(strcmp(argv[1],"-r")==0){
        close(inputFd);
    }
    else if((strcmp(argv[1],"-a")==0) || (strcmp(argv[1],"-a")==0)){
        close(outputFd);
    }
    exit(EXIT_SUCCESS);
}