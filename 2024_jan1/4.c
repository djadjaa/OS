#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <utime.h>
#include <sys/time.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \


extern char **environ;
int usr1=0,usr2=0,stop=0;

void signalHandler(int signum){
    switch(signum){
        case SIGUSR1:
            usr1=1;
            break;
        case SIGUSR2:
            usr2=1;
            break;
        case SIGQUIT:
            stop=1;
            break;
    }
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    //printf("%d\n",getpid());
    char *value = getenv("PUTANJA");
    //printf("%s\n",value);
    
    do{
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGQUIT,signalHandler)!=SIG_ERR,"signal");

        pause();

        struct stat fInfo;
        checkError(stat(value,&fInfo)!=-1,"stat");
        if(usr1==1){
            char result[4];
            if(fInfo.st_mode & S_IRUSR)
                result[0]='r';
            else
                result[0]='-';
            if(fInfo.st_mode & S_IWUSR)
                result[1]='w';
            else
                result[1]='-';
            if(fInfo.st_mode & S_IXUSR)
                result[2]='x';
            else
                result[2]='-';
            printf("%s\n",result);
            usr1=0;
        }else if(usr2==1){
            time_t numsec = strtol(argv[1],NULL,10);
            /*
            struct tm* time = localtime(&numsec);
            checkError(time!=NULL,"local time");
             */
            
            struct utimbuf vreme;
            vreme.actime = numsec;
            vreme.modtime = numsec;

            checkError(utime(value,&vreme)!=-1,"utime");
            usr2=0;
        }
    }while(!stop);

    exit(EXIT_SUCCESS);
}
