#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define checkError(expr,userMsg) \
    do{\
        if(!(expr)){\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);\

#define WR_END (1)
#define RD_END (0)
int main(int argc, char **argv){
    int pajp[2];
    checkError(pipe(pajp)!=-1,"pipe");
    pid_t childPid=fork();
    checkError(childPid!=-1,"fork");
    if(childPid==0){ //child process
        close(pajp[RD_END]);
        checkError(dup2(pajp[WR_END],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("ls", "ls", "-l",(char*)NULL),"execlp");
        exit(EXIT_SUCCESS);
    }else{ //parent process
        close(pajp[WR_END]);
        FILE *fd=fdopen(pajp[RD_END],"r");
        checkError(fd!=NULL,"fdopen");
        char *line=NULL;
        size_t readBytes=0;
        char pravaPristupa[11]={0};//prazan string
        char rez[11]={0};
        int retval = getline(&line, &readBytes, fd);
        //ignorisemo prvu liniju
        line = NULL;
        readBytes = 0;
        int status;
        checkError(wait(&status)!=-1,"wait");
        if(!(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS)){
            printf("neuspeh\n");
            return WEXITSTATUS(status); //vracamo exit kod koji vraca ls komanda
        }else{
            while(getline(&line,&readBytes,fd)!=-1){
                sscanf(line,"%s",pravaPristupa);
                //drwxrwxrwx
                if(!strncmp(pravaPristupa+7, "rwx", 3)){ //prava+7 -> idemo na sedmo mesto niske
                    sscanf(line, "%*s%*s%*s%*s%*s%*s%*s%*s%s", rez);
                    printf("%s\n",rez);
                }
            }
        }
        free(line);
        fclose(fd);
    }

    exit(EXIT_SUCCESS);
}
