#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <string.h>
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

    char *dirname;
    scanf("%ms",&dirname);
    strcat(dirname,".dir");
    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"");
    pid_t childPid = fork();
    checkError(childPid!=-1,"");

    if(childPid==0){
        close(cld2par[RD]);

        checkError(dup2(STDIN_FILENO,cld2par[WR])!=-1,"");

        mode_t prava = 0777;
        mode_t oldumask = umask(0);

        checkError(mkdir(dirname,prava)!=-1,"");

        umask(oldumask);

        close(cld2par[WR]);
        exit(EXIT_SUCCESS);
    }else{
        close(cld2par[WR]);

        int status=0;
        checkError(wait(&status)!=-1,"");

        if(!WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS)
            return WEXITSTATUS(status);

        close(cld2par[RD]);
    }

    free(dirname);

    exit(EXIT_SUCCESS);
}
