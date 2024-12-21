#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <libgen.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#define checkError(expr,usrMsg) \
    do{ \
        if (!(expr)){ \
            perror(usrMsg); \
            exit(1); \
        } \
    }while(0)

int usr1caught=0, usr2caught=0, stop=0;

void signalHandler(int signum){
    switch(signum){
        case SIGUSR1:
            usr1caught=1;
            break;
        case SIGUSR2:
            usr2caught=1;
            break;
        case SIGQUIT:
            stop=1;
            break;
    }
}

int main(int argc, char **argv){

    checkError(argc==1,"args");
    char *path;
    do{
        checkError(signal(SIGQUIT,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"signal");
        pause();

        scanf("%ms",&path);

        char *file = basename(path);
        struct stat fInfo;
        checkError(stat(file,&fInfo)!=-1,"fstat");
        if(usr1caught){
            printf("%ld ",fInfo.st_size);
            usr1caught=0;
        }else if(usr2caught){
            //dan u mesecu:
            struct tm *time = localtime(&(fInfo.st_mtime));
            checkError(time!=NULL,"localtime");
            printf("%d ",time->tm_mon);
            usr2caught=0;
        }

        free(path);
    }while(!stop);
    exit(EXIT_SUCCESS);
}