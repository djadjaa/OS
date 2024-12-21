#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <stdbool.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

bool containsDot(const char *s){
    int i=0;
    while(s[i]!='\0'){
        if(s[i]=='.')
            return true;
        i++;
    }
    return false;
}

int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    if(S_ISDIR(sb->st_mode)){
        if(ftwbuf->level >=1 && ftwbuf->level <= 3){
            if(containsDot(fpath+ftwbuf->base)){
                printf("%s\n",fpath+ftwbuf->base);
            }
        }
    }

    return 0;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"not folder");
    checkError(nftw(argv[1],processDir,50,FTW_PHYS)!=-1,"nftw");
    exit(EXIT_SUCCESS);
}