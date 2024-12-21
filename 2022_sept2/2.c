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
    int numLines=0, maxCh=0, numChLongLine=0;
    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"pipe");
    pid_t childPid = fork();
    checkError(childPid!=-1,"fork");
    if(childPid==0){
        close(cld2par[RD]);
        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("cat","cat",argv[1],(char*)NULL)!=1,"execlp");
        exit(EXIT_SUCCESS);
    }else{
        close(cld2par[WR]);
        FILE *f = fdopen(cld2par[RD],"r");
        checkError(f!=NULL,"fdopen");
        char *line=NULL;
        size_t len=0;
        int numch=0;
        while((numch=getline(&line,&len,f))>0){
            numLines++;
            if(numch>maxCh){
                maxCh=numch;
                numChLongLine=0;
                for(int i=0; line[i]!='\0'; i++)
                    numChLongLine++;
            }
            free(line);
        }

        int status=0;
        checkError(wait(&status)!=-1,"wait");

        fclose(f);
    }
    printf("%d %d\n", numLines, numChLongLine);
    exit(EXIT_SUCCESS);
}