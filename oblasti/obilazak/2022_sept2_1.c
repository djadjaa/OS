#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int size = 0;

int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    
    if(S_ISREG(sb->st_mode)){
        //if((sb->st_mode & S_IXUSR) || (sb->st_mode & S_IXGRP) || (sb->st_mode & S_IXOTH)){
        if(sb->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)){
            size += sb->st_size;
        }
    }
    
    return 0;
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"");
/*
    int nftw(const char *dirpath,
               int (*fn) (const char *fpath, const struct stat *sb,
                          int typeflag, struct FTW *ftwbuf),
               int nopenfd, int flags);
     */

    checkError(nftw(argv[1],processDir,50,0)!=-1,"");
    printf("%d\n",size);

    exit(EXIT_SUCCESS);
}