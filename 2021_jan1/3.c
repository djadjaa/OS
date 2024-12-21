#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 1024

int main(int argc, char **argv){

    checkError(argc==2,"args");
    struct stat lInfo;
    checkError(lstat(argv[1],&lInfo)!=-1,"");
    checkError(S_ISLNK(lInfo.st_mode),"link");
    char *buff=malloc(MAX*sizeof(char));
    checkError(buff!=NULL,"");
    checkError(readlink(argv[1],buff,MAX)!=-1,"");

    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");

    if(S_ISREG(fInfo.st_mode))
        printf("regularan ");
    else if(S_ISDIR(fInfo.st_mode))
        printf("direktorijum ");
    else if(S_ISCHR(fInfo.st_mode))
        printf("karakter ");
    else if(S_ISBLK(fInfo.st_mode))
        printf("blok ");
    else if(S_ISFIFO(fInfo.st_mode))
        printf("fifo/pajp ");
    else if(S_ISLNK(fInfo.st_mode))
        printf("link ");
    else if(S_ISSOCK(fInfo.st_mode))
        printf("socket ");
    
    char *ext = strrchr(buff,'.');
    if(ext!=NULL)
        printf("%s",ext);
    else
        printf("nema");
    putchar('\n');
    free(buff);
    exit(EXIT_SUCCESS);
}
