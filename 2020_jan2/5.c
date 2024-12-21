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

void print_env(char *wrd){
    char **ep;
    int n=strlen(wrd);
    char niska[n+1];
    for(ep=environ; *ep!=NULL; ep++){
        strncpy(niska,*ep,n);
        if(strcmp(niska,wrd)==0){
            char* stampa = strchr(*ep,'=');
            printf("%s ", stampa+1);
        }
    }
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    char *wrd = argv[1];
    int n = strlen(wrd);
    int num=0;
    for(int i=0; i<n; i++){
        if(isupper(wrd[i]))
            num++;
    }
    if(num==n)
        print_env(wrd);
    else
        checkError(0,"not upper");

    exit(EXIT_SUCCESS);
}
