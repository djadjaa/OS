#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

bool prestupna(int year){
    if (year % 4 == 0) {
        if (year % 100 == 0) {
            if (year % 400 == 0) {
                return true;
            } else {
                return false;
            }
        } else {
            return true;
        }
    } else {
        return false;
    }
}
/*
int najblizaPrestupna(int year){
    int lower = year, upper = year;
    while(1){
        if(prestupna(lower))
            return lower;
        else if(prestupna(upper))
            return upper;
        lower--;
        upper++;
    }
}
*/


int main(int argc, char **argv){

    checkError(argc==2,"args");
    //br sek od pocetka epohe
    time_t numsec = strtol(argv[1],NULL,10);
    struct tm *time=localtime(&numsec);
    checkError(time!=NULL,"localtime");

    int year = time->tm_year + 1900;

    while(1){
        if(!prestupna(year)) //ako nije prestupna idemo godinu nize dokle god ne naidjemo na prestupnu
            year--;
        else //inace smo je pronasli i prekidamo sa petljom
            break;
    }

    
    printf("%d\n",year);
    exit(EXIT_SUCCESS);
}
