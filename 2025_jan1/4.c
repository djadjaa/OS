#define _DEFAULT_SOURCE
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <libgen.h>

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int usr1=0, stop=0;

void signalHandler(int signum){
    switch (signum)
    {
    case SIGUSR1:
        usr1 = 1;
        break;
    case SIGQUIT:
        stop=1;
        break;
    }
}

int main(int argc, char **argv){
    printf("%d\n",getpid());
    char *path;
    do{
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"");
        checkError(signal(SIGQUIT,signalHandler)!=SIG_ERR,"");
        pause();
        scanf("%ms",&path);
        if(usr1){
            char *fname = basename(path);
            struct stat fInfo;
            checkError(stat(fname,&fInfo)!=-1,"");
            checkError(S_ISREG(fInfo.st_mode),"");
            printf("%ld ",fInfo.st_size);
            usr1=0;
        }
    }while(!stop);
    putchar('\n');
    exit(EXIT_SUCCESS);
}
