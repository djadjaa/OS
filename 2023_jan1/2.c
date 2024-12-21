#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ftw.h>
#include <string.h>
#include <time.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

char *ext = NULL;
size_t res=0;

int processDir(const char *fpath, const struct stat *sb,int typeflag, struct FTW *ftwbuf){
    if(typeflag==FTW_F){
        time_t now = time(NULL);
        checkError(now!=-1,"time");
        int timeDif = (now - sb->st_mtime)/(24*60*60);
        if(timeDif<=7){
            char *exttmp = strrchr(fpath+ftwbuf->base,'.');
            if(exttmp!=NULL && strcmp(exttmp,ext)==0){
                res += sb->st_size;
            }
        }
    }
    return 0;
}

int main(int argc,char **argv){

    checkError(argc==3,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"folder");
    ext = argv[2];
    checkError(nftw(argv[1],processDir,50,0)!=-1,"nftw");
    printf("%ld\n",res);
    exit(EXIT_SUCCESS);
}