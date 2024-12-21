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
#define MAX 255
int main(int argc, char **argv){

    checkError(argc==2,"args");
    FILE *input = fopen(argv[1],"r");
    checkError(input!=NULL,"fopen");
    char command[MAX] = {0};
    char argument[MAX] = {0};
    int maxCharErr=-1;
    char res[MAX];
    while(fscanf(input,"%s%s",command,argument)!=EOF){
        int charErr=0;
        int cld2par[2];
        checkError(pipe(cld2par)!=-1,"pipe");
        pid_t childPid=fork();
        checkError(childPid!=-1,"fork");
        if(childPid==0){
            close(cld2par[RD]);
            checkError(dup2(open("/dev/null",O_WRONLY),STDOUT_FILENO)!=-1,"dup2");
            //checkError(dup2(cld2par[WR],STDERR_FILENO)!=-1,"dup2");
            //checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
            checkError(execlp(command,command,argument,(char*)NULL)!=-1,"execlp");
            exit(EXIT_SUCCESS);
        }else{
            close(cld2par[WR]);
            
            FILE *fd = fdopen(cld2par[RD],"r");
            checkError(fd!=NULL,"fdopen");
            
            int status=0;
            checkError(wait(&status)!=-1,"wait");
            if(!(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS)){
                
                char *line=NULL;
                size_t len = 0;
                int totalRead=0;
                while(getline(&line,&len,fd)>0)
                    totalRead+=len;

                if(totalRead>maxCharErr){
                    maxCharErr=totalRead;
                    strcpy(res,command);
                }
                
            }
            //fclose(fd);
        }
    }

    if(maxCharErr!=-1)
        fprintf(stdout,"%s %d\n",res,maxCharErr);
    else
        fprintf(stdout,"unknown 0\n");

    fclose(input);

    exit(EXIT_SUCCESS);
}