#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(lstat(argv[1],&fInfo)!=-1,"lstat");
    checkError(S_ISLNK(fInfo.st_mode),"sym link");
    printf("%d\n",(int)fInfo.st_mtime);

    exit(EXIT_SUCCESS);
}