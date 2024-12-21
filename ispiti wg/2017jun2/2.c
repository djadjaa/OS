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

    checkError(argc==2,"args");
    char *extension=argv[1];
    int pajp[2];
    checkError(pipe(pajp)!=-1,"pipe");
    pid_t childPid=fork();
    checkError(childPid!=-1,"fork");
    if(childPid==0){ //child process
        close(pajp[RD_END]);
        checkError(dup2(pajp[WR_END],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("find", "find", ".", "-type", "f", (char*)NULL)!=-1,"execlp");
        exit(EXIT_SUCCESS);
    }else{ //parent process;
        close(pajp[WR_END]);
        FILE *fd=fdopen(pajp[RD_END],"r");
        checkError(fd!=NULL,"fdopen");
        char *line=NULL;
        char *tmp=NULL;
        size_t readBytes=0;
        int extLen=strlen(extension); //velicina ekstenzije
        int br=0; //brojac koji broji koliko fajlova ima sa tom ekstenzijom
        while(getline(&line,&readBytes,fd)!=-1){ //sve dok se cita linija
            tmp=strrchr(line,'.'); //izdvaja se ona sa tackom
            if(tmp!=NULL && strncmp(tmp,extension,extLen)==0){ //ako je dobro alocirana privremena linija i ako je jednaka sa ekstenzijom
                br++;
            }
        }
        printf("%d\n",br);
        free(line);
        fclose(fd);
        int status=0;
        checkError(wait(&status)!=-1,"wait");
    }

    exit(EXIT_SUCCESS);
}
