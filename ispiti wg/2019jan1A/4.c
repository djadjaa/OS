#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX 256
#define RDEND 0
#define WREND 1

#define checkError(expr,userMsg) \
    do{\
        if(!(expr)){\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\

int main(int argc, char **argv){

    checkError(argc==2,"args");
    FILE *f=fopen(argv[1],"r");
    checkError(f!=NULL,"fopen");
    int readLine=0;
    size_t len=0;
    char *line=NULL;
    char rez[MAX];
    int maxBytes=0;
    while((readLine=getline(&line,&len,f))!=-1){
        line[readLine]='\0';
        int pajp[2];
        checkError(pipe(pajp)!=-1,"pipe");
        pid_t childPid=fork();
        if(childPid==0){ //child process
            close(pajp[RDEND]);
            checkError(dup2(pajp[WREND],STDOUT_FILENO)!=-1,"dup2");
            char komanda[MAX]={0}; //inicijalizacija
            char argument[MAX]={0};
            sscanf(line,"%s%s",komanda,argument);
            checkError(execlp(komanda,komanda,argument,(char*)NULL)!=-1,"execlp");
            exit(EXIT_SUCCESS);
        }else{ //parent process
            close(pajp[WREND]);
            int countBytes=0;
            char buffer[MAX];
            int readBytes=0;
            while((readBytes=read(pajp[RDEND],buffer,sizeof(buffer)))>0){
                countBytes += readBytes;
            }
            checkError(readBytes!=-1,"read");
            if(countBytes>maxBytes){
                maxBytes=countBytes;
                strcpy(rez,line);
            }
            int status=0;
            checkError(wait(&status)!=-1,"wait");
        }
        free(line);
    }
    printf("%s\n",rez);
    fclose(f);
    exit(EXIT_SUCCESS);
}
