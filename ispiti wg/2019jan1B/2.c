#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define checkError(expr,userMsg) \
    do{\
        if(!(expr)){\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\

int stop=0,usr1c=0,usr2c=0,intc=0;

void signalHandler(int signum){
    switch(signum){
        case SIGUSR1:
            usr1c=1;
            break;
        case SIGUSR2:
            usr2c=1;
            break;
        case SIGINT:
            intc=1;
            break;
        case SIGTERM:
            stop=1;
            break;
    }
}

int main(int argc, char **argv){
    int score=0;
    do{
        checkError(signal(SIGTERM,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGINT,signalHandler)!=SIG_ERR,"signal");
        if(usr1c){
            score += 1;
            usr1c=0;
        }else if(usr2c){
            score += 2;
            usr2c=0;
        }else if(intc){
            score -= 4;
            intc=0;
        }
    }while(!stop);
    printf("%d\n",score);
    exit(EXIT_SUCCESS);
}
