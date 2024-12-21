#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <errno.h>

#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);


int main(int argc,char **argv){

    checkError(argc==4,"args");
    mode_t oldUmask = umask(0);
    mode_t pravaPristupa = strtol(argv[3],NULL,8);
    int res;
    if(strcmp(argv[1],"-f")==0){ //regularan fajl
        res = open(argv[2],O_RDONLY|O_CREAT,pravaPristupa);
        if(res==-1){
            if(errno!=EEXIST){
                checkError(res!=-1,"open");
            }
        }
        close(res);
    }else if(strcmp(argv[1],"-d")==0){ //direktorijum
        res=mkdir(argv[2],pravaPristupa);
        if(res==-1){
            if(errno!=EEXIST){
                checkError(res!=-1,"mkdir");
            }
        }
    }else{
        checkError(0,"wrong choice");
    }
    //ako postoji
    checkError(chmod(argv[2],pravaPristupa)!=-1,"chmod");
    umask(oldUmask);
    exit(EXIT_SUCCESS);
}