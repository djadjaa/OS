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

int main(int argc, char **argv){

    checkError(argc==2,"args");
    //br sek od pocetka epohe
    time_t numsec = strtol(argv[1],NULL,10);
    struct tm *time = localtime(&numsec);
    checkError(time!=NULL,"time");

    //pocetak meseca
    struct tm *startOfMonth = time;
    startOfMonth->tm_mday = 1;
    startOfMonth->tm_hour = 0;
    startOfMonth->tm_min = 0;
    startOfMonth->tm_sec = 0;

    //menjamo vreme
    time_t newTime= mktime(startOfMonth);
    checkError(newTime!=-1,"mktime");

    //br sek i sati od pocetka meseca
    //fja za razliku od tekuceg dana u mesecu do pocetka meseca
    int numinsec = difftime(numsec,newTime);
    int numinhours = numinsec/3600;
    printf("%d\n",numinhours);

    exit(EXIT_SUCCESS);
}
