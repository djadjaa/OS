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

int main(int argc, char **argv){

    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"pipe");
    pid_t childPid = fork();
    checkError(childPid!=-1,"fork");
    if(childPid==0){
        close(cld2par[RD]);
        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("pwd","pwd",(char*)NULL)!=-1,"execlp");
        exit(EXIT_SUCCESS);
    }else{
        close(cld2par[WR]);
        FILE *f = fdopen(cld2par[RD],"r");
        checkError(f!=NULL,"fdopen");
        
        char *line=NULL;
        size_t len=0;
        int read=getline(&line,&len,f);
        checkError(read!=-1,"getline");
        char *path=malloc(len+1);
        checkError(path!=NULL,"path");
        strncpy(path,line,6);
        if(strcmp(path,"/home/")==0){
            char *name = strtok(line+6,"/");
            fprintf(stdout,"%s\n",name);
        }else
            fprintf(stdout,"system\n");
        

        int status=0;
        checkError(wait(&status)!=-1,"wait");
        free(path);
        free(line);
        fclose(f);
    }


    exit(EXIT_SUCCESS);
}