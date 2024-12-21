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

    checkError(argc==3,"args");
    time_t now;
    checkError(time(&now)!=-1,"time");
    time_t numSec=strtol(argv[1],NULL,10);
    time_t numMin=strtol(argv[2],NULL,10);
    struct tm *time=localtime(&numSec);
    checkError(time!=NULL,"local time");
    time->tm_min += numMin;
    time_t newTime=mktime(time);
    char timeString[18];
    // DD/MM/YYYY HH:MM
    strftime(timeString,17,"%d/%m/%Y %H:%M",time);
    printf("%s\n", timeString);
    exit(EXIT_SUCCESS);
}
