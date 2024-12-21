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
    mode_t pravaPristupa = strtol(argv[3],NULL,8);
    mode_t stariUmask = umask(0);

    if(strcmp(argv[1],"-f")==0){
        int fd = open(argv[2],O_RDWR|O_CREAT|O_EXCL,pravaPristupa);
        /*
         O_EXCL Ensure  that  this  call  creates the file: if this flag is specified in conjunction with O_CREAT, and pathname already exists, then open()
              fails with the error EEXIST
        */
        checkError(fd!=-1,"open");
        close(fd);
    }else if(strcmp(argv[1],"-d")==0){
        checkError(mkdir(argv[2],pravaPristupa)!=-1,"mkdir");
    }else
        checkError(0,"wrong choice");

    umask(stariUmask);
    //ako postoji vracamo 1
    exit(EXIT_SUCCESS);
}