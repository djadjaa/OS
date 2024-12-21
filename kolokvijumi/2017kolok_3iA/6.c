#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

#define MAX 8096

int main(int argc,char **argv){

    checkError(argc==3,"args");
    char *src = realpath(argv[1],NULL);
    char *dest = realpath(argv[2],NULL);
    checkError(src!=NULL,"src must exist");
    if(src!=NULL && dest!=NULL)
        checkError(strcmp(src,dest)!=0,"equal");
    free(src);
    free(dest);
    checkError(rename(argv[1],argv[2])!=-1,"mv");

    exit(EXIT_SUCCESS);
}
//sta radi rename sa regularnim fajlovima: 1.txt 2.txt -> 1.txt se brise i prebacuje se sadrzaj u 2.txt a prethodni sadrzaj iz 2.txt se brise