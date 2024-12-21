#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <time.h>
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

char *ext = NULL;
int now = 0;

int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    //Ako je regularan fajl
    if(typeflag==FTW_F){
        int timeDif = (now-sb->st_mtime)/(24*60*60);
        if(timeDif <= 7){
            char *tacka = strrchr(fpath+ftwbuf->base,'.');
            if(tacka!=NULL && strcmp(tacka,ext)==0)
                printf("%s\n",fpath+ftwbuf->base);
        }
    }

    return 0;
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"not dir");
    ext = argv[2];
    checkError(time(&now)!=-1,"time");
    checkError(nftw(argv[1],processDir,50,0)!=-1,"nftw");

    exit(EXIT_SUCCESS);
}