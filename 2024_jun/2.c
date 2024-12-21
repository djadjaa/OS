#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <string.h>
#include <time.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

char *ext=NULL;
int size=0;

int processDir(const char *fpath, const struct stat *sb,  int typeflag, struct FTW *ftwbuf){

    if(S_ISREG(sb->st_mode)){
        time_t now = time(NULL);
        checkError(now!=-1,"");
        int timeDif = (now-sb->st_mtime)/(24*60*60);
        if(timeDif<=7){
            char *exttmp=strrchr(fpath+ftwbuf->base,'.');
            if(exttmp!=NULL && strcmp(exttmp,ext)==0){
                size += sb->st_size;
            }
        }
    }

    return 0;
}

int main(int argc, char **argv){

    checkError(argc==3,"");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"");
    ext=argv[2];
    checkError(nftw(argv[1],processDir,50,0)!=-1,"");
    printf("%d\n",size);

    exit(EXIT_SUCCESS);
}
