#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);


int main(int argc,char **argv){

    checkError(argc==3,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"STAT");
    char *pname = strrchr(argv[1],'/');
    double size=0;
    if(strcmp(argv[2],"KB")==0){
        size = (double)((fInfo.st_size*1.0)/1024);
    }
    else if(strcmp(argv[2],"MB")==0){
        size = (double)((fInfo.st_size*1.0)/(1024*1024));
    }
    else if(strcmp(argv[2],"GB")==0){
        size = (double)((fInfo.st_size*1.0)/(1024*104*1024));
    }
    else
        checkError(0,"wrong choice");
    size = ceil(size);
    printf("%s %d%s",(pname!=NULL ? pname+1 : argv[1]),(int)size,argv[2]); //ovo smo uradili ako nemamo datu apsolutnu putanju

    exit(EXIT_SUCCESS);
}