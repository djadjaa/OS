#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <libgen.h>
#include <pwd.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int usr1=0, stop=0;

void signalHandler(int signum){
    switch(signum){
        case SIGUSR1:
            usr1=1;
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
        scanf("%ms",&path);
        pause();
        if(usr1){
            char *fname = basename(path);
            struct stat fInfo;
            checkError(stat(fname,&fInfo)!=-1,"");
            struct passwd *usr = getpwuid(fInfo.st_uid);
            printf("%s ",usr->pw_name);
            usr1=0;
        }
        free(path);
    }while(!stop);
    exit(EXIT_SUCCESS);
}
