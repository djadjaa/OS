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


int main(int argc,char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    struct stat lInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(lstat(argv[1],&lInfo)!=-1,"lstat");
    checkError(S_ISREG(fInfo.st_mode),"regular file");
    checkError(S_ISLNK(lInfo.st_mode),"symbolic link");
    size_t size_f = fInfo.st_size;
    size_t size_l = lInfo.st_size;
    printf("%ld\n", size_f-size_l);

    exit(EXIT_SUCCESS);
}