#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define checkError(expr,usrMsg) \
    do { \
        if(!(expr)){ \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0); \

void satnica(int hour){
    if(hour >= 0 && hour < 7){
        printf("noc\n");
    }
    else if(hour >= 7 && hour < 9){
        printf("jutro\n");
    }
    else if(hour >=9 && hour < 12){
        printf("prepodne\n");
    }
    else if(hour >= 12 && hour < 19){
        printf("popodne\n");
    }
    else if(hour >= 19 && hour <= 23){
        printf("vece\n");
    }
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    time_t inputTime = strtol(argv[1],NULL,10);
    //checkError(time(&inputTime)!=-1,"time");

    struct tm *timeNow = localtime(&inputTime);
    checkError(timeNow!=NULL,"localtime");

    satnica(timeNow->tm_hour);

    exit(EXIT_SUCCESS);
}
