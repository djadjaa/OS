#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>

#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

#define SIZE 8096
void osMoveFile(const char *srcPath, const char *destPath){

    /*
    otvorimo src fajl za citanje a dest za pisanje i dok citamo iz src fajla premestamo sadrzaj iz src u dest pisanjem
    */

    char *src = realpath(srcPath,NULL);
    char *dest = realpath(destPath,NULL);
    checkError(src!=NULL," src"); //mora postojati source fajl iz kog prebacujemo sadrzaj
    if(src!=NULL && dest!=NULL) {//ako postoje oba fajla
        checkError(strcmp(src,dest)!=0,"equal"); //ako su jednaki
    } 
        
    free(src);
    free(dest);

    struct stat fInfo;
    checkError(stat(srcPath,&fInfo)!=-1,"stat");

    int srcfd=open(srcPath,O_RDONLY);
    checkError(srcfd!=-1,"open");
    mode_t stariUmask = umask(0);
    int destfd = open(destPath,O_WRONLY|O_CREAT|O_TRUNC,fInfo.st_mode);
    umask(stariUmask);
    checkError(destfd!=-1,"open");
    int readBytes=0;
    char buff[SIZE];
    while((readBytes=read(srcfd,buff,SIZE))>0){
        checkError(write(destfd,buff,readBytes)!=-1,"write");
    }
    checkError(readBytes!=-1,"read");
    close(srcfd);
    close(destfd);
}

int main(int argc,char **argv){

    checkError(argc==3,"args");
    osMoveFile(argv[1],argv[2]);

    exit(EXIT_SUCCESS);
}