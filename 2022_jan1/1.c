#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \


#define MAX 8096

int main(int argc, char **argv){

    checkError(argc==3,"args");
    char *srcPath = realpath(argv[1],NULL);
    char *destPath = realpath(argv[2],NULL);
    checkError(srcPath!=NULL,"src must exists");
    if(srcPath!=NULL && destPath!=NULL)
        checkError(strcmp(srcPath,destPath)!=0,"src and dest cant be same");
    free(srcPath);
    free(destPath);

    struct stat fInfo;
    //src
    checkError(stat(argv[1],&fInfo)!=-1,"stat"); 
    int fdsrc = open(argv[1],O_RDONLY);
    checkError(fdsrc!=-1,"open");

    //dest
    mode_t stariUmask = umask(0);

    int fddest = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC, fInfo.st_mode);
    checkError(fddest!=-1,"open");

    umask(stariUmask);

    char *buff = malloc(MAX+1);
    checkError(buff!=NULL,"malloc");

    int readBytes=0;
    while((readBytes = read(fdsrc,buff,MAX))>0){
        checkError(write(fddest,buff,readBytes)!=-1,"write");
    }
    checkError(readBytes!=-1,"read");
    free(buff);
    close(fdsrc);
    close(fddest);
    exit(EXIT_SUCCESS);
}