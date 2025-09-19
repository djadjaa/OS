#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

void monthName(time_t mon){
    switch(mon){
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

int main(int argc, char **argv){

    checkError(argc==3,"args");
    time_t numsec = strtol(argv[1],NULL,10);
    int nummonths = atoi(argv[2]);
    struct tm *time = localtime(&numsec);
    //potrebno je ispisati nazive meseca trenutno i izmenjen naziv meseca
    monthName(time->tm_mon);
    time->tm_mon -= nummonths;
    checkError(mktime(time)!=-1,"");
    monthName(time->tm_mon);

    exit(EXIT_SUCCESS);
}
