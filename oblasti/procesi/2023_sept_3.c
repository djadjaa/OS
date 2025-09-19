#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define RD 0
#define WR 1
#define MAX 1024
int main(int argc, char **argv){

    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"");
    pid_t childPid = fork();
    checkError(childPid!=-1,"");
    if(childPid==0){
        close(cld2par[RD]);

        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"");
        checkError(execlp("pwd","pwd",(char*)NULL)!=-1,"");

        close(cld2par[WR]);
        exit(EXIT_SUCCESS);
    }else{
        close(cld2par[WR]);

        /*
        char buff[MAX];
        int readBytes=0, readRn=0;
        while((readRn=read(cld2par[RD],buff+readBytes,MAX-readBytes))>0)
            readBytes+=readRn;
        buff[readBytes]='\0';
        printf("%d\n",readBytes-1);*/
        char *line;
        size_t len=0;
        FILE *f = fdopen(cld2par[RD],"r");
        checkError(f!=NULL,"");
        checkError(getline(&line,&len,f)!=-1,"");
        printf("%ld\n",strlen(line)-1);
        free(line);
        fclose(f);
        close(cld2par[RD]);
    }
    int status=0;
    checkError(wait(&status)!=-1,"");
    exit(EXIT_SUCCESS);
}
