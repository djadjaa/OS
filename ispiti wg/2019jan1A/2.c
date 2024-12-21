#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#define checkError(expr,userMsg) \
    do{\
        if(!(expr)){\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\

int usr1Caught=0,usr2Caught=0,stop=0;

void signalHandler(int signum){
    switch(signum){
        case SIGUSR1:
            usr1Caught=1;
            break;
        case SIGUSR2:
            usr2Caught=1;
            break;
        case SIGTERM:
            stop=1;
            break;
    }
}

int main(int argc, char **argv){
    int n;
    do{
        checkError(signal(SIGTERM,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"signal");
        pause();
        scanf("%d",&n);
        if(usr1Caught){
            printf("%d\n",abs(n));
            usr1Caught=0;
        }else if(usr2Caught){
            printf("%d\n",n*n);
            usr2Caught=0;
        }
    }while(!stop);

    exit(EXIT_SUCCESS);
}
