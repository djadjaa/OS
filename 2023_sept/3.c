#define _POSIX_C_SOURCE 2

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#define checkError(expr,userMsg) \
    do { \
        if(!(expr)){ \
            perror(userMsg); \
            exit(1); \
        } \
    }while(0)

#define WR 1
#define RD 0
#define MAX 4096
int main(int argc,char **argv){

    checkError(argc==1,"args");
    int pajp[2];
    checkError(pipe(pajp)!=-1,"pipe");
    pid_t child = fork();
    checkError(child!=-1,"fork");
    if(child==0) { //dete
        close(pajp[RD]);
        dup2(pajp[WR], STDOUT_FILENO);
        checkError(execlp("pwd","pwd",(char*)NULL)!=-1,"execvp");
        exit(EXIT_SUCCESS);

    }else{ //roditelj
        wait(NULL);
        close(pajp[WR]);
        char buf[MAX];
        int readBytes=0, readNow = 0;
        while((readNow = read(pajp[RD], buf + readBytes, MAX - readBytes)) > 0) {
            readBytes += readNow;
        }
        buf[readBytes] = '\0';

        printf("%d\n", readBytes - 1);
    }
    exit(EXIT_SUCCESS);
}
