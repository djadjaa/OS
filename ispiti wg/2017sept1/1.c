#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <ctype.h>
#include <string.h>

#define MAX (64)

#define checkError(expr,userMsg) \
    do{\
        if(!(expr)){\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);\

int stop=0;
int usr1Caught=0,usr2Caught=0;

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

void reverse(char *s){
    int n=strlen(s),tmp;
    for(int i=0; i<n/2; i++){
        tmp=s[i];
        s[i]=s[n-i-1];
        s[n-i-1]=tmp;
    }
}

void bigLetters(char *s){
    for(int i=0; s[i]!='\0'; i++){
        if(islower(s[i])){
            s[i]=toupper(s[i]);
        }
    }
}

int main(int argc, char **argv){
    char word[MAX];
    do{
        checkError(signal(SIGTERM,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
        checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"signal");
        pause();
        scanf("%s",word);
        if(usr1Caught){
            reverse(word);
            printf("%s\n",word);
            usr1Caught=0;
        }else if(usr2Caught){
            bigLetters(word);
            printf("%s\n",word);
            usr2Caught=0;
        }
    }while(!stop);

    exit(EXIT_SUCCESS);
}
