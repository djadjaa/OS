#define _XOPEN_SOURCE 700
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
    uid_t uid_f = fInfo.st_uid;
    uid_t uid_l = lInfo.st_uid;
    gid_t gid_f = fInfo.st_gid;
    gid_t gid_l = lInfo.st_gid;

    if(uid_f == uid_l && gid_f == gid_l)
        printf("da\n");
    else
        printf("ne\n");

    exit(EXIT_SUCCESS);
}