#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define RD 0
#define WR 1
#define MAX 256

int main(int argc, char **argv){
    checkError(argc==1,"args");
    
    char *line=NULL;
    char *res=NULL;
    size_t len=0;
    int failed=0, maxCh=INT_MIN;
    while(getline(&line,&len,stdin)>0){
        int n;
        int ukprocitano=0, procitano=0;
        //iz linije izvlacimo broj n i koliko je bajtova procitano za sad do broja n; zanemarujemo razmak
        sscanf(line+ukprocitano,"%d%*c%n", &n,&procitano);
        ukprocitano+=procitano;
        char **args = (char**)malloc((n+1)*sizeof(char*));
        checkError(args!=NULL,"malloc");
        for(int i=0; i<n; i++){
            args[i] = (char*)malloc(MAX*sizeof(char));
            checkError(args[i]!=NULL,"malloc args[i]");
        }
        //izvlacimo argumente i koliko je bajtova procitano, opet zanemarujemo razmake
        for(int i=0; i<n; i++){
            sscanf(line+ukprocitano,"%s%*c%n", args[i],&procitano);
            ukprocitano+=procitano;
        }
        args[n] = NULL;
        int cld2par[2];
        checkError(pipe(cld2par)!=-1,"pipe");
        pid_t childPid = fork();
        checkError(childPid!=-1,"fork");
        if(childPid==0){
            close(cld2par[RD]);

            checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
            checkError(execvp(args[0],args)!=-1,"execvp");

            close(cld2par[WR]);
            exit(EXIT_SUCCESS);
        }else{
            close(cld2par[WR]);
            
            int status=0;
            checkError(wait(&status)!=-1,"wait");
            if(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS){
                int readBytes=0, totalReadBytes=0;
                char buff[MAX];
                while((readBytes=read(cld2par[RD],buff,MAX))>0)
                    totalReadBytes+=readBytes;
                checkError(readBytes!=-1,"read");
                if(totalReadBytes>maxCh){
                    maxCh=totalReadBytes;
                    res=malloc(strlen(args[0])*sizeof(char));
                    checkError(res!=NULL,"malloc res");
                    strcpy(res,args[0]);
                }
            }else{
                failed++;
            }
            close(cld2par[RD]);
            for(int i=0; i<n; i++)
                free(args[i]);
            free(args);
        }
    }
    printf("%d %s\n",failed,(maxCh != INT_MIN ? res : "nema"));
    free(res);
    free(line);
    exit(EXIT_SUCCESS);
}
