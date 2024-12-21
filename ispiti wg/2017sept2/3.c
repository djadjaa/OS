#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L
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

    char *line=NULL;
    ssize_t readBytes=0;
    int x,y;
    char op[2];
    while(getline(&line,&readBytes,stdin)!=-1){
        sscanf(line,"%d %d %s", &x,&y,op);
        int xLen = snprintf(NULL, 0, "%d", x); //printf vraca broj istampanih karaktera pa ce xLen biti 1
        char *xStr=malloc(xLen+1);
        snprintf(xStr,xLen+1,"%d",x);
        //int snprintf(char *str, size_t size, const char *format, ...);
        int yLen = snprintf(NULL, 0, "%d", y);
        char *yStr=malloc(yLen+1);
        snprintf(yStr,yLen+1,"%d",y);
        int pajp[2];
        checkError(pipe(pajp)!=-1,"pipe");
        pid_t childPid=fork();
        checkError(childPid!=-1,"fork");
        if(childPid==0){ //child process
            close(pajp[RDEND]);
            //dup2 i execlp
            checkError(dup2(pajp[WREND],STDOUT_FILENO)!=-1,"dup2");
            checkError(execlp("expr","expr",xStr,op,yStr,(char*)NULL),"execlp");
            exit(EXIT_SUCCESS);
        }else{ //parent process
            close(pajp[WREND]);
            FILE *fd=fdopen(pajp[RDEND],"r");
            checkError(fd!=NULL,"fdopen");
            char *lineTmp=NULL;
            size_t readBytesTmp=0;
            int x=getline(&lineTmp,&readBytesTmp,fd);
   //         lineTmp[readBytesTmp]=0; // postavljanje terminirajuce nule
            checkError(x!=-1,"getline");
            int status=0;
            checkError(wait(&status)!=-1,"wait");
            printf("%s",lineTmp); //stampamo rezultat
            free(lineTmp);
            fclose(fd);
        }
    }

    exit(EXIT_SUCCESS);
}
