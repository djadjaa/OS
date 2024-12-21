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

void dayInWeek(time_t day){
    switch (day){
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
        case 0:
            printf("nedelja ");
            break;
    }
        
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    time_t numsecEpoch = strtol(argv[1],NULL,10);
    int numyears = atoi(argv[2]);
    struct tm *time = localtime(&numsecEpoch);
    checkError(time!=NULL,"localtime");
    
    dayInWeek(time->tm_wday);

    time->tm_year += numyears;
    time_t newTime = mktime(time);
    checkError(newTime!=-1,"mktime");

    dayInWeek(time->tm_wday);

    exit(EXIT_SUCCESS);
}
