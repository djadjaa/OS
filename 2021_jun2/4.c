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
    int maxNumBytes=0;
    char *line=NULL;
    size_t len=0;
    int readLine=0;
    char res[MAX];
    while((readLine=getline(&line,&len,f))>0){
        line[readLine]='\0';
        int cld2par[2];
        checkError(pipe(cld2par)!=-1,"");
        pid_t childPid = fork();
        checkError(childPid!=-1,"");
        if(childPid==0){
            close(cld2par[RD]);
            checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"");

            char command[MAX]={0};
            char args[MAX]={0};

            sscanf(line,"%s%s",command,args);
            checkError(execlp(command,command,args,NULL)!=-1,"execlp");

            exit(EXIT_SUCCESS);
        }else{
            close(cld2par[WR]);
            int numBytes=0;
            char buff[MAX];
            int readBytes=0;
            while((readBytes=read(cld2par[RD],buff,MAX))>0){
                numBytes+=readBytes;
            }
            checkError(readBytes!=-1,"readbytes");
            if(numBytes>maxNumBytes){
                maxNumBytes=numBytes;
                strcpy(res,line);
            }
            free(line);
        }
    }
    printf("%s\n",res);
    checkError(wait(NULL)!=-1,"");
    fclose(f);
    exit(EXIT_SUCCESS);
}
