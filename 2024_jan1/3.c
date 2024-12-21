#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define checkError(expr,usrMsg) \
    do{ \
        if (!(expr)){ \
            perror(usrMsg); \
            exit(1); \
        } \
    }while(0)

#define RD 0
#define WR 1
int main(int argc, char **argv){

    checkError(argc==1,"args");
    char *potDir;
    scanf("%ms",&potDir);
    strcat(potDir,".dir");
    int pajp[2];
    checkError(pipe(pajp)!=-1,"pipe");
    pid_t child = fork();
    checkError(child!=-1,"fork");

    if(child==0){ //dete proces
        close(pajp[RD]);
        dup2(STDIN_FILENO,pajp[WR]);
        mode_t pravapristupa = 0777;
        mode_t oldumask = umask(0);
        checkError(mkdir(potDir,pravapristupa)!=-1,"mkdir");
        umask(oldumask);
        exit(EXIT_SUCCESS);
    }//roditelj proces
    close(pajp[WR]);
    int wstatus;
    checkError(wait(&wstatus)!=-1,"wait");
    if(!(WIFEXITED(wstatus) && WEXITSTATUS(wstatus)==EXIT_SUCCESS)){
        return WEXITSTATUS(wstatus);
    }
    free(potDir);
    
    exit(EXIT_SUCCESS);
}