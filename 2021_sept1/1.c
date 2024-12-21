#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

void monthName(time_t month){
    switch (month){
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
        case 0:
            printf("januar ");
            break;
    }
        
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    time_t numSecEpoch = strtol(argv[1],NULL,10);
    int numdays =  atoi(argv[2]);
    struct tm *time = localtime(&numSecEpoch);
    checkError(time!=NULL,"localtime");

    monthName(time->tm_mon);

    time->tm_mday += numdays;
    time_t newTime = mktime(time);
    checkError(newTime!=-1,"mktime");

    monthName(time->tm_mon);
    exit(EXIT_SUCCESS);
}
