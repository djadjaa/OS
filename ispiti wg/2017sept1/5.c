#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

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

#define RDEND 0
#define WREND 1
int main(int argc, char **argv){

    checkError(argc==2,"args");
    int pajp[2];
    checkError(pipe(pajp)!=-1,"pipe");
    pid_t childPid=fork();
    checkError(childPid!=-1,"fork");
    if(childPid==0){ //child process
        close(pajp[RDEND]);
        checkError(dup2(pajp[WREND],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("ls","ls","-l",argv[1],(char*)NULL),"execlp");
        exit(EXIT_SUCCESS);
    }else{ //parent process
        close(pajp[WREND]);
        FILE *fd=fdopen(pajp[RDEND],"r");
        char *line=NULL;
        size_t readBytes=0;
        int x=0;
        while((x=getline(&line,&readBytes,fd))!=-1){
            char *tmpLine=malloc(x+1);
            checkError(tmpLine!=NULL,"malloc");
            sscanf(line, "%*s%*s%*s%*s%*s%*s%*s%s", tmpLine); //izvlacimo sate
            printf("%s\n", tmpLine);
            free(tmpLine);
        }
        int status=0;
        checkError(wait(&status)!=-1,"wait");
        free(line);
        fclose(fd);
    }
    exit(EXIT_SUCCESS);
}
