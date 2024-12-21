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

void danUNedelji(int dan){
    switch(dan){
        case 0:
            printf("nedelja\n");
            break;
        case 1:
            printf("ponedeljak\n");
            break;
        case 2:
            printf("utorak\n");
            break;
        case 3:
            printf("sreda\n");
            break;
        case 4:
            printf("cetvrtak\n");
            break;
        case 5:
            printf("petak\n");
            break;
        case 6:
            printf("subota\n");
            break;
    }
}
int main(int argc, char **argv){
    checkError(argc==2,"args");
    time_t now;
    checkError(time(&now)!=-1,"time");

    time_t numSec=strtol(argv[1],NULL,10);

    struct tm *time = localtime(&numSec);
    checkError(time != NULL, "localtime failed");
    danUNedelji(time->tm_wday);
    exit(EXIT_SUCCESS);
}
