#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
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
    checkError(argc==3,"args");
    int max=0;
    char *res=NULL;
    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"pipe");
    pid_t childPid = fork();
    checkError(childPid!=-1,"fork");
    if(childPid==0){
        close(cld2par[RD]);
        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("head","head","-n",argv[2],argv[1],(char*)NULL)!=-1,"execlp");
        exit(EXIT_SUCCESS);
    }else{
        close(cld2par[WR]);

        int status=0;
        checkError(wait(&status)!=-1,"wait");

        if(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS){
            
            FILE *f = fdopen(cld2par[RD],"r");
            checkError(f!=NULL,"fdopen");
            char *line=NULL;
            size_t len=0;
            int read=0;
            while((read=getline(&line,&len,f))>0){
                res = malloc((len+1)*sizeof(char));
                checkError(res!=NULL,"malloc");

                //printf("%d\n",read);
                if(read>max){
                    max = read;
                    strcpy(res,line);
                }

                
                free(line);
            }
            
            
            printf("%s",res);
            fclose(f);
        }

    }
    
    free(res);
    exit(EXIT_SUCCESS);
}