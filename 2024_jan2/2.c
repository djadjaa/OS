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

extern char **environ;
int main(int argc, char **argv){

    char *path = getenv("REG_FAJL");
    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"pipe");
    pid_t childPid = fork();
    checkError(childPid!=-1,"fork");
    if(childPid==0) {
        close(cld2par[RD]);
        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"DUP2");
        checkError(execlp("cat","cat",path,NULL)!=-1,"execlp");
    }else{
        close(cld2par[WR]);

        FILE *f = fdopen(cld2par[RD],"r");
        checkError(f!=NULL,"fdopen");

        char *line=NULL;
        size_t len=0;
        int read=0;
        int maxread=0;

        while((read=getline(&line,&len,f))>0){
            if(read>maxread)
                maxread = read;
        }
        printf("%d\n",maxread-1);
        free(line);
        fclose(f);
    }
    checkError(wait(NULL),"wait");
    exit(EXIT_SUCCESS);
}
