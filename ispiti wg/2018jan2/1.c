#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>

#define checkError(expr,userMsg) \
    do{\
        if(!(expr)){\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\

int main(int argc, char **argv){

    checkError(argc==2,"args");
    time_t now;
    checkError(time(&now)!=-1,"time");
    time_t numSec=strtol(argv[1],NULL,10);
    struct tm *time=localtime(&numSec);
    checkError(time!=NULL,"local time");
    char timeString[6];
    strftime(timeString,6,"%H:%M",time);
    //printf("%H:%d\n",time->tm_hour,time->tm_min);
    printf("%s\n",timeString);
}
