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

int stop=0;
int usr1caught=0,usr2caught=0;
void signalHandler(int signum){
    switch(signum){
        case SIGUSR1:
            usr1caught=1;
            break;
        case SIGUSR2:
            usr2caught=1;
            break;
        case SIGTERM:
            stop=1;
            break;
    }
}

int main(int argc, char **argv){
    int numusr1=0,numusr2=0;
    do{
        checkError(signal(SIGTERM,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"signal");
        pause();
        if(usr1caught){
            numusr1++;
            usr1caught=0;
        }
        if(usr2caught){
            numusr2++;
            usr2caught=0;
        }
    }while(!stop);

    exit(EXIT_SUCCESS);
}
