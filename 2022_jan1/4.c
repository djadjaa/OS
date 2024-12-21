#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX_LINE 255
#define RD 0
#define WR 1

int main(int argc, char **argv){
    checkError(argc==1,"args");

    int pajp[2];
    checkError(pipe(pajp)!=-1,"pipe");
    char *line = NULL;
    char *res=NULL;
    size_t len = 0;
    int neuspesnoPokr=0, maxKar = -1;
    while(getline(&line,&len,stdin)>0){
        int n;
        int ukprocitano=0,procitano=0;
        sscanf(line+ukprocitano,"%d%*c%n",&n,&procitano); //izvlacimo broj n
        ukprocitano+=procitano;
        char **argumenti = (char**)malloc((n+1)*sizeof(char*)); //br argumenata komandne linije
        checkError(argumenti!=NULL,"malloc");
        for(int i=0; i<n; i++){
            argumenti[i] = (char*)malloc((MAX_LINE+1)*sizeof(char));
            checkError(argumenti[i]!=NULL,"malloc");
        }
        for(int i=0;i<n; i++){
            sscanf(line+ukprocitano,"%s%*c%n", argumenti[i],&procitano); //izvlacimo koji su argumenti u pitanju
            ukprocitano+=procitano;
        }
        argumenti[n]=NULL;

        int cld2par[2];
        checkError(pipe(cld2par)!=-1,"pipe");
        pid_t childPid = fork();
        checkError(childPid!=-1,"fork");

        if(childPid==0){
            close(cld2par[RD]);
            checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
            checkError(execvp(argumenti[0],argumenti)!=-1,"execvp");
            exit(EXIT_SUCCESS);
        }
        else{
            close(cld2par[WR]);
            int status=0;
            checkError(wait(&status)!=-1,"wait");
            if(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS){
                int readBytes=0, totalReadBytes=0;
                char buff[MAX_LINE];
                while((readBytes=read(cld2par[RD],buff,MAX_LINE))>0)
                    totalReadBytes+=readBytes;
                checkError(readBytes!=-1,"read");
                if(totalReadBytes>maxKar){
                    maxKar=totalReadBytes;
                    res=malloc(strlen(argumenti[0])*sizeof(char));
                    checkError(res!=NULL,"malloc");
                    strcpy(res,argumenti[0]);
                }
            }else
                neuspesnoPokr++;
            close(cld2par[RD]);
            for(int i=0; i<n; i++)
                free(argumenti[i]);
            free(argumenti);
        }
    }
    
    fprintf(stdout,"%d %s\n",neuspesnoPokr,(maxKar!=-1 ? res : "nema"));
    free(res);
    free(line);
    exit(EXIT_SUCCESS);
}