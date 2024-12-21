#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int usr1=0,usr2=0,stop=0;

void signalHandler(int signum){
    switch(signum){
        case SIGUSR1:
            usr1=1;
            break;
        case SIGUSR2:
            usr2=1;
            break;
        case SIGTERM:
            stop=1;
            break;
    }
}

int palindrom(char *s){
    int start=0, end = strlen(s)-1;
    while(start < end){
        if(s[start]!=s[end])
            return 0;
        start++;
        end--;
    }
    return 1;
}

void reverse(char *s){
    int i=0, j=strlen(s)-1;
    char tmp;
    while(i<j){
        tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;

        i++;
        j--;
    }
}

int main(int argc, char **argv){

    char *s=NULL;
    do{
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGTERM,signalHandler)!=SIG_ERR,"signal");
        pause();
        scanf("%ms\n",&s);
        if(usr1){
            printf("%d ",palindrom(s));
            usr1=0;
        }
        else if(usr2){
            reverse(s);
            printf("%s ",s);
            usr2=0;
        }
    }while(!stop);
    exit(EXIT_SUCCESS);
}