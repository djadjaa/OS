#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <utime.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){

    checkError(argc==3,"args");
    int numsec = strtol(argv[2],NULL,10);
    struct utimbuf vreme;
    vreme.actime = numsec;
    vreme.modtime = numsec;
    checkError(utime(argv[1],&vreme)!=-1,"utime");


    exit(EXIT_SUCCESS);
}