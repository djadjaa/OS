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
#define MAX 256
int main(int argc, char **argv){

    checkError(argc==2,"args");
    FILE *f = fopen(argv[1],"r");
    checkError(f!=NULL,"fopen");
    int readline=0;
    size_t len=0;
    char *line=NULL;
    char res[MAX];
    int maxNumBytes=0;
    while((readline=getline(&line,&len,f))!=-1){
        line[readline]='\0';
        int cld2par[2];
        checkError(pipe(cld2par)!=-1,"pipe");
        pid_t childPid = fork();
        checkError(childPid!=-1,"fork");
        if(childPid==0){
            close(cld2par[RD]);
            checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
            char command[MAX]={0};
            char argument[MAX]={0};
            sscanf(line,"%s%s",command,argument); //izvlacimo iz stringa argument i komandu
            checkError(execlp(command,command,argument,(char*)NULL)!=-1,"execvp");
            exit(EXIT_SUCCESS);
        }else{
            close(cld2par[WR]);
            int count=0;
            char buff[MAX];
            int readBytes=0;
            while((readBytes=read(cld2par[RD],buff,MAX))>0)
                count += readBytes;
            checkError(readBytes!=-1,"read");
            if(count > maxNumBytes){
                maxNumBytes = count;
                strcpy(res,line);
            }
            int status=0;
            checkError(wait(&status)!=-1,"wait");
        }
        free(line);
    }

    printf("%s\n",res);
    fclose(f);
    exit(EXIT_SUCCESS);
}