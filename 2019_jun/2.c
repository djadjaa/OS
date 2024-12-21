#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
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

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"pipe");
    pid_t childPid = fork();
    checkError(childPid!=-1,"fork");
    if(childPid==0){ //child process
        close(cld2par[RD]);
        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("stat","stat","--format=%A",argv[1], (char*)NULL)!=-1,"execlp");
        exit(EXIT_SUCCESS);
    }else{ //parent process
        close(cld2par[WR]);
        FILE *fd = fdopen(cld2par[RD],"r");
        checkError(fd!=NULL,"fdopen");

        size_t readBytes=0;
        char *line = NULL;
        int x = getline(&line,&readBytes,fd);
        int status = 0;

        checkError(wait(&status)!=-1,"wait");

        if(!(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS) && x==-1){
            printf("neuspeh\n");
            exit(EXIT_SUCCESS);
        }
        printf("%s",line);
        free(line);
        fclose(fd);
    }

    exit(EXIT_SUCCESS);
}