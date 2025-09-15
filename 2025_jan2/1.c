#define _XOPEN_SOURCE 700

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>

#define checkError(expr,usrMsg) \
    do { \
        if(!(expr)) { \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0); \

char *pathtest = "test_";

int filter(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){

    if(S_ISDIR(sb->st_mode)){
        if(strncmp(pathtest,fpath+ftwbuf->base,strlen(pathtest))==0){
            if((sb->st_mode & S_IWGRP) && (sb->st_mode & S_IWOTH))
                printf("%s\n", fpath+ftwbuf->base);
        }
    }

    return 0;
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"");
    checkError(S_ISDIR(fInfo.st_mode),"");
    checkError(nftw(argv[1],filter,50,0)!=-1,"");

    exit(EXIT_SUCCESS);
}