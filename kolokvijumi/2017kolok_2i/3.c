#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

unsigned osNumOfDaysFileModified(const char *fpath){
    struct stat fInfo;
    checkError(stat(fpath,&fInfo)!=-1,"stat");
    time_t now=time(NULL);
    checkError(now!=-1,"time");
    unsigned numDays=(now-fInfo.st_mtime)/(24*60*60);
    return numDays;
}

int main(int argc,char **argv){

    checkError(argc==2,"args");
    printf("%u\n", osNumOfDaysFileModified(argv[1]));

    exit(EXIT_SUCCESS);
}