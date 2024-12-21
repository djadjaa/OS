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

void meseci(int months){
    switch(months){
        case 0:
            printf("januar\n");
            break;
        case 1:
            printf("februar\n");
            break;
        case 2:
            printf("mart\n");
            break;
        case 3:
            printf("april\n");
            break;
        case 4:
            printf("maj\n");
            break;
        case 5:
            printf("jun\n");
            break;
        case 6:
            printf("jul\n");
            break;
        case 7:
            printf("avgust\n");
            break;
        case 8:
            printf("septembar\n");
            break;
        case 9:
            printf("oktobar\n");
            break;
        case 10:
            printf("novembar\n");
            break;
        case 11:
            printf("decembar\n");
            break;
    }
}

int main(int argc, char **argv){
    checkError(argc==2,"args");
    time_t now;
    checkError(time(&now)!=-1,"time");
    time_t numSec=strtol(argv[1],NULL,10);
    struct tm *time=localtime(&numSec);
    checkError(time!=NULL,"local time");
    meseci(time->tm_mon);
    exit(EXIT_SUCCESS);
}
