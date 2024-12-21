#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);


int main(int argc,char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISREG(fInfo.st_mode),"regular");
    unsigned access = fInfo.st_atime;
    unsigned modification = fInfo.st_mtime;
    unsigned rez = (access-modification)/(24*60*60);
    printf("%u\n",rez);

    exit(EXIT_SUCCESS);
}