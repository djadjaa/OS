#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int usr1caught=0, usr2caught=0, stop=0;

void signalHandler(int signum){
    switch (signum)
    {
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

    int a;
    do{
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"signla");
        checkError(signal(SIGTERM,signalHandler)!=SIG_ERR,"signal");
        pause();
        scanf("%d\n",&a);
        if(usr1caught){
            printf("%d\n",abs(a));
            usr1caught=0;
        }
        else if(usr2caught){
            printf("%d\n",a*a);
            usr2caught=0;
        }
    }while(!stop);
    

    exit(EXIT_SUCCESS);
}