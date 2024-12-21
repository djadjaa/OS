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

void daniUNedelji(int days){
    switch(days){
        case 0:
            printf("nedelja ");
            break;
        case 1:
            printf("ponedeljak ");
            break;
        case 2:
            printf("utorak ");
            break;
        case 3:
            printf("sreda ");
            break;
        case 4:
            printf("cetvrtak ");
            break;
        case 5:
            printf("petak ");
            break;
        case 6:
            printf("subota ");
            break;
    }
}
int main(int argc, char **argv){
    checkError(argc==3,"args");
    time_t now;
    checkError(time(&now)!=-1,"time");
    time_t numSec = atoi(argv[1]);
    time_t numYr = atoi(argv[2]);
    struct tm *time=localtime(&numSec);
    checkError(time!=NULL,"local time");
    daniUNedelji(time->tm_wday);

    time->tm_year += numYr;
    time_t newTime=mktime(time);
    daniUNedelji(time->tm_wday);

    exit(EXIT_SUCCESS);
}
