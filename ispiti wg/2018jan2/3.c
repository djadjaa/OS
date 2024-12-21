#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
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

    checkError(argc==3,"args");
    int pajp[2];
    checkError(pipe(pajp)!=-1,"pipe");
    pid_t childPid=fork();
    checkError(childPid!=-1,"fork");
    if(childPid==0){
        close(pajp[RD_END]);
        checkError(dup2(pajp[WR_END],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("tail", "tail", "-n",argv[2],argv[1],(char*)NULL),"execlp");
        exit(EXIT_SUCCESS);
    }else{
        close(pajp[WR_END]);
        FILE *fd=fdopen(pajp[RD_END],"r");
        checkError(fd!=NULL,"fdopen");
        char *line=NULL;
        size_t readBytes=0;
        while(getline(&line,&readBytes,fd)!=-1){
            printf("%s",line);
        }
        free(line);
        fclose(fd);
    }
    int status=0;
    checkError(wait(&status)!=-1,"wait");
    if(!(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS)){
        printf("Neuspeh\n");
    }
    exit(EXIT_SUCCESS);
}
