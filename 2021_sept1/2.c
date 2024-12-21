#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int usr1=0,usr2=0,intsig=0,stop=0;

void signalHandler(int signum){
    switch(signum){
        case SIGUSR1:
            usr1=1;
            break;
        case SIGUSR2:
            usr2=1;
            break;
        case SIGINT:
            intsig=1;
            break;
        case SIGTERM:
            stop=1;
            break;
    }
}

int main(int argc, char **argv){

    int score=0;
    do{
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGINT,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGTERM,signalHandler)!=SIG_ERR,"signal");
        pause();
        if(usr1){
            score++;
            usr1=0;
        }
        else if(usr2){
            score += 2;
            usr2=0;
        }
        else if(intsig){
            score -= 4;
            intsig=0;
        }
    }while(!stop);
    printf("%d\n",score);

    exit(EXIT_SUCCESS);
}