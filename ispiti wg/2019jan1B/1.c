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

void meseciUGodini(int months){
    switch(months){
        case 0:
            printf("januar ");
            break;
        case 1:
            printf("februar ");
            break;
        case 2:
            printf("mart ");
            break;
        case 3:
            printf("april ");
            break;
        case 4:
            printf("maj ");
            break;
        case 5:
            printf("jun ");
            break;
        case 6:
            printf("jul ");
            break;
        case 7:
            printf("avgust ");
            break;
        case 8:
            printf("septembar ");
            break;
        case 9:
            printf("oktobar ");
            break;
        case 10:
            printf("novembar ");
            break;
        case 11:
            printf("decembar ");
            break;
    }
}

int main(int argc,char **argv){

    checkError(argc==3,"args");
    time_t now;
    checkError(time(&now)!=-1,"time");
    time_t numSec=atoi(argv[1]);
    struct tm *time=localtime(&numSec);
    checkError(time!=NULL,"local time");
    meseciUGodini(time->tm_mon);
    time_t numDays=atoi(argv[2]);
    time->tm_mday += numDays;
    time_t newTime = mktime(time);
    meseciUGodini(time->tm_mon);
    exit(EXIT_SUCCESS);
}
