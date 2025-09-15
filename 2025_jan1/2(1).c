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

#define RD 0
#define WR 1
#define MAX 4096

int main(int argc, char **argv){

    checkError(argc==1,"");
    char *path;
    scanf("%ms",&path);
    int res=0;
    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"");
    pid_t childPid = fork();
    checkError(childPid!=-1,"");
    if(childPid==0){ //dete proces
        close(cld2par[RD]);

        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"");
        checkError(execlp("wc","wc","-l",path,(char*)NULL)!=-1,"");

        close(cld2par[WR]);
    }else{ //roditelj proces
        close(cld2par[WR]);

        char buff[MAX];
        int readBytes = read(cld2par[RD],buff,MAX-1);
        checkError(readBytes!=-1,"");
        buff[readBytes]='\0';

        int numLines;
        sscanf(buff,"%d",&numLines);
        res += numLines;
        printf("%d\n",res);
        close(cld2par[RD]);
    }

    int status=0;
    checkError(wait(&status)!=-1,"");
    free(path);
    exit(EXIT_SUCCESS);
}
