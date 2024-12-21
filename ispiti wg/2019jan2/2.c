#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
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
    
    checkError(argc>=2,"args");
    for(int i=1; i<argc; i++){
        int cld2par[2];
        checkError(pipe(cld2par)!=-1,"pipe");
        pid_t childPid = fork();
        checkError(childPid!=-1,"childpid");
        if(childPid==0){
            close(cld2par[RD]);

            checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
            checkError(execlp("du","du", "-sch",argv[i],(char*)NULL)!=-1,"execlp");

            close(cld2par[WR]);
            exit(EXIT_SUCCESS);
        }
        else{
            close(cld2par[WR]);

            int status=0;
            checkError(wait(&status)!=-1,"wait");
            if(!(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS))
                printf("neuspeh");
            else{
                while(1){
                    char c;
                    read(cld2par[RD],&c,1);
                    
                    if(isspace(c))
                        break;
                    
                    if(c=='.')
                        c=',';
                    putchar(c);
                }
            }
            putchar(' ');

            close(cld2par[RD]);
        }
    }
    
        
    exit(EXIT_SUCCESS);
}