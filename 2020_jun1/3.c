#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

extern char **environ;

void printenv(char *s){
    char **ep;
    int n = strlen(s);
    char str[n+1];
    for(ep=environ; *ep!=NULL; ep++){
        strncpy(str,*ep,n);
        if(strcmp(s,str)==0){
            char *res = strchr(*ep,'=');
            printf("%s ",res+1);
        }
    }
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    int n = strlen(argv[1]);
    int numtmp=0;
    for(int i=0; i<n; i++){
        if(isupper(argv[1][i]))
            numtmp++;
    }
    if(numtmp==n)
        printenv(argv[1]);
    else
        checkError(0,"");

    exit(EXIT_SUCCESS);
}
