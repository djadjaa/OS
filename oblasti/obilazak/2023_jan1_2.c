#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <string.h>
#include <time.h>
#define checkError(expr, usrMsg) \
    do{\
        if(!(expr)){ \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

char *ext = NULL;
int size=0;

int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){

    if(S_ISREG(sb->st_mode)){
        time_t now = time(NULL);
        checkError(now!=-1,"time");
        int timedif = (now - sb->st_mtime)/(24*60*60);
        if(timedif <= 7){ //ako je modifikovan fajl u poslednjih 7 dana
            char *exttmp = strrchr(fpath+ftwbuf->base,'.');
            if(exttmp!=NULL && strcmp(exttmp,ext)==0){ //ako je to trazena ekstenzija
                size += sb->st_size;
            }
        }
    }
    return 0;
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    ext = argv[2];
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"not dir")
    checkError(nftw(argv[1],processDir,50,0)!=-1,"");
    printf("%d\n",size);
    /*
    int nftw(const char *dirpath,
               int (*fn) (const char *fpath, const struct stat *sb,
                          int typeflag, struct FTW *ftwbuf),
               int nopenfd, int flags);
     */

    exit(EXIT_SUCCESS);
}