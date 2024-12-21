#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

char *ext=NULL;

/*
struct FTW {
               int base;
               int level;
           };

*/

int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){

    if(S_ISREG(sb->st_mode)){
        //izmedju drugog i petog nivoa
        if(ftwbuf->level >= 2 && ftwbuf->level <=5){
            char *extmp = strrchr(fpath,'.');
            if(extmp!=NULL && strcmp(ext,extmp)==0){
                printf("%s\n",fpath+ftwbuf->base);
            }
        }
    }

    return 0;
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"folder");
    ext = argv[2];
    checkError(nftw(argv[1],processDir,50,0)!=-1,"nftw");

    exit(EXIT_SUCCESS);
}