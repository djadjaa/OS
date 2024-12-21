#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>

#define MAX 1024

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
    //readlink(2)
    char *buff = malloc(MAX*sizeof(char));
    checkError(buff!=NULL,"malloc");
    checkError(readlink(argv[1],buff,MAX)!=-1,"readlink");
    printf("%s\n",buff);
    free(buff);
    exit(EXIT_SUCCESS);
}