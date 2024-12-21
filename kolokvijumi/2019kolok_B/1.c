#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);


int main(int argc,char **argv){

    checkError(argc==3,"args");
    struct stat fInfo1;
    struct stat fInfo2;
    checkError(stat(argv[1],&fInfo1)!=-1,"stat");
    checkError(stat(argv[2],&fInfo2)!=-1,"stat");
    time_t time1 = fInfo1.st_mtime;
    time_t time2 = fInfo2.st_mtime;
    printf("%s\n", (time1 > time2) ? basename(argv[1]) : basename(argv[2]));

    exit(EXIT_SUCCESS);
}