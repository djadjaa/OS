#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define checkError(expr,userMsg) \
    do{\
        if(!(expr)){\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);\

#define WR_END (1)
#define RD_END (0)

int main(int argc, char **argv){

    checkError(argc>1,"args");
    char *komanda = argv[1];
    char **argumenti = malloc(argc*sizeof(char*));
    for(int i=0; i<argc; i++){
        argumenti[i]=argv[i+1];
    }
    argumenti[argc-1]=NULL;//enter
    int pajp[2];
    checkError(pipe(pajp)!=-1,"pipe");
    pid_t childPid = fork();
    checkError(childPid!=-1,"fork");

    int numLines=0;
    //broj linija na izlazu pokrenutog programa
    if(childPid==0){ //child process
        close(pajp[RD_END]);
        checkError(dup2(pajp[WR_END],STDOUT_FILENO)!=-1,"dup2");
        //checkError(execlp(argumenti,argumenti,komanda,)!=-1,"execlp");
        checkError(execvp(komanda, argumenti)!=-1,"execvp");
        exit(EXIT_SUCCESS);
    }else{ //parent process
        close(pajp[WR_END]);
        FILE *f = fdopen(pajp[RD_END],"r");
        checkError(f!=NULL,"fdopen");
        char *line=NULL;
        size_t len=0;
        while(getline(&line,&len,f)!=-1){
            numLines++;
        }
        free(line);
        fclose(f);
    }

    int status=0;
    checkError(wait(&status)!=-1,"wait");
    if(!(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS)){
        printf("Neuspeh\n");
        exit(EXIT_FAILURE);
    }
    printf("%d\n",numLines);
    free(argumenti);
    exit(EXIT_SUCCESS);
}
