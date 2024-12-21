#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <libgen.h>
#include <pwd.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int usr1=0,stop=0;

void signalHandler(int signum){
    switch(signum){
        case SIGUSR1:
            usr1=1;
            break;
        case SIGTERM:
            stop=1;
            break;
    }
}



int main(int argc, char **argv){
    char *path;
    do{
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGTERM,signalHandler)!=SIG_ERR,"signal");
        pause();
        scanf("%ms",path);
        char *fajl = basename(path);
        if(usr1){
            struct stat fInfo;
            checkError(stat(fajl,&fInfo)!=-1,"stat");
            struct passwd *usr = getpwuid(fInfo.st_uid);
            checkError(usr!=NULL,"getpwuid");
            printf("%s ",usr->pw_name);
            usr1=0;
        }

    }while(!stop);
    exit(EXIT_SUCCESS);
}