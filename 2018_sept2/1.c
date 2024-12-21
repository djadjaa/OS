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

int usr1=0,usr2=0,term=0;

void signalHandler(int signum){
    switch (signum){
    case SIGUSR1:
        usr1=1;
        break;
    case SIGUSR2:
        usr2=1;
        break;
    case SIGTERM:
        term=1;
        break;
    }
}

int main(int argc, char **argv){

    int numusr1=0,numusr2=0;
    do{
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"");
        checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"");
        checkError(signal(SIGTERM,signalHandler)!=SIG_ERR,"");
        pause();
        if(usr1){
            numusr1++;
            usr1=0;
        }
        else if(usr2){
            numusr2++;
            usr2=0;
        }
    }while(!(term));
    printf("%d %d\n",numusr1,numusr2);

    exit(EXIT_SUCCESS);
}
