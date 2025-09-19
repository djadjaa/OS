#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <string.h>
#include <libgen.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){

    if(S_ISDIR(sb->st_mode)){
        if(strstr(fpath+ftwbuf->base,".git")!=NULL){
            char *res = realpath(fpath,NULL);
            checkError(res!=NULL,"");
            //printf("%s\n",res);

            const char *slash = strrchr(res, '/');
            if (slash != NULL) {
                // duzina roditeljskog puta
                size_t len = slash - res;
                // ispis roditeljskog direktorijuma
                printf("%.*s\n", (int)len, res);
            }

            free(res);
        }

    }
    return 0;
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"");
    checkError(S_ISDIR(fInfo.st_mode),"");
    checkError(nftw(argv[1],processDir,50,0)!=-1,"");


    exit(EXIT_SUCCESS);
}
