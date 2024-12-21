#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
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
    }while(0);

#define RD_END (0)
#define WR_END (1)
#define MAXSIZE (1024)

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int pajp[2];
    checkError(pipe(pajp)!=-1,"pipe");
    pid_t childPid=fork();
    checkError(childPid!=-1,"child process");
    if(childPid==0){//dete proces
        close(pajp[RD_END]);
        checkError(dup2(pajp[WR_END],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("ls","ls","-l",argv[1],(char*)NULL)!=-1,"execlp");
        exit(EXIT_SUCCESS);
    }else{ //roditelj proces
        close(pajp[WR_END]);
        FILE *fd=fdopen(pajp[RD_END],"r");
        checkError(fd!=NULL,"fd");
        size_t readBytes;
        char *line=NULL;
        int x=getline(&line,&readBytes,fd);
        int status=0;
        checkError(wait(&status)!=-1,"wait");
         if(!(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS) && x==-1){
            printf("Neuspeh\n");
            exit(EXIT_SUCCESS); // <=> exit(0)
        }
        //string mora da ima 10 karaktera
        char rez[12];
        snprintf(rez,11,"%s\n",line);
        printf("%s\n",rez);
        free(line);
        fclose(fd);
    }

    return 0;
}
