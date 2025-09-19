#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <pwd.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){

    if(S_ISFIFO(sb->st_mode)){
        if((sb->st_mode & S_IRGRP) && (sb->st_mode & S_IWGRP)){
            struct passwd *usr = getpwuid(sb->st_uid);
            char *res = realpath(fpath,NULL);
            checkError(res!=NULL,"");
            printf("%s: %s\n", res, usr->pw_name);
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
