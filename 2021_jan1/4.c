#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <ftw.h>
#include <stdlib.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

char *ext=NULL;
int p,k;
int num=0;
int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    
    if(S_ISREG(sb->st_mode)){
        if(ftwbuf->level >= p && ftwbuf->level <= k){
            char *exttmp = strrchr(fpath,'.');
            if(exttmp!=NULL && strcmp(exttmp,ext)==0)
                num++;
        }
    }
    
    return 0;
}

int main(int argc, char **argv){

    checkError(argc==5,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"folder");
    ext = argv[2];
    p = atoi(argv[3]);
    k = atoi(argv[4]);
    checkError(nftw(argv[1],processDir,50,0)!=-1,"");
    printf("%d\n",num);
    exit(EXIT_SUCCESS);
}
