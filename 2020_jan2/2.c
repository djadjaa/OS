#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
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

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int num=0;
    char *ext = argv[1];
    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"pipe");
    pid_t childPid = fork();
    checkError(childPid!=-1,"fork");
    if(childPid==0){
        close(cld2par[RD]);
        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("find","find",".","-type","f",(char*)NULL)!=-1,"execvp");
        exit(EXIT_SUCCESS);
    }else{
        close(cld2par[WR]);
        FILE *f = fdopen(cld2par[RD],"r");
        checkError(f!=NULL,"fdopen");

        size_t readBytes=0;
        char *line = NULL;
        int extlen = strlen(ext);
        char *tmpext = NULL;
        int num=0;
        while(getline(&line,&readBytes,f)!=-1){
            tmpext = strrchr(line,'.');
            if(tmpext!=NULL && strncmp(tmpext,ext,extlen)==0)
                num++;
        }

        printf("%d\n",num);
        free(line);
        fclose(f);
        int status = 0;
        checkError(wait(&status)!=-1,"wait");
        
    }

    exit(EXIT_SUCCESS);
}