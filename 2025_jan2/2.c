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
#define MAX 256

int main(int argc, char **argv){

    checkError(argc==2,"");
    int numLines=0;
    int maxNumCh=0;
    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"");
    pid_t childPid = fork();
    checkError(childPid!=-1,"");
    if(childPid==0){ //child
        close(cld2par[RD]);

        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"");
        checkError(execlp("cat","cat",argv[1],(char*)NULL)!=-1,"");

        close(cld2par[WR]);
    }else{ //parent
        close(cld2par[WR]);

        FILE *f = fdopen(cld2par[RD],"r");
        checkError(f!=NULL,"");

        char *line=NULL;
        size_t len=0;
        int read=0;
        while((read=getline(&line,&len,f))>0){
            numLines++;
            if(read>maxNumCh)
                maxNumCh = read;
        }
        printf("%d %d\n",numLines,maxNumCh-1);
        free(line);
        fclose(f);
        close(cld2par[RD]);
    }

    int status = 0;
    checkError(wait(&status)!=-1,"");
    exit(EXIT_SUCCESS);
}
