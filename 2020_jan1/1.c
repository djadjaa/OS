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

int main(int argc, char **argv){

    checkError(argc==2,"args");
    //br sek od pocetka epohe
    time_t numsec = strtol(argv[1],NULL,10);
    struct tm *time = localtime(&numsec);
    checkError(time!=NULL,"localtime");

    //br sek od pocetka prethodne nedelje
    int dan = time->tm_wday;
    int sat = time->tm_hour;
    int min = time->tm_min;
    int sec = time->tm_sec;

    time_t numseclastweek = (dan == 0 ? 6 : time->tm_wday - 1)*86400+sat*3600+min*60+sec;
    time_t numhourslastweek = numseclastweek/3600;
    printf("%ld\n",numhourslastweek);
    exit(EXIT_SUCCESS);
}
