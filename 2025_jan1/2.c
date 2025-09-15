#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define RD 0
#define WR 1
#define MAX 4096

#define checkError(expr, usrMsg) \
    do{ \
        if(!(expr)){ \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0); \
    
int main(int argc, char **argv){

    checkError(argc==1,"");

    int numLines=0;
    char *path;
    scanf("%ms",&path);
    int cld2par[2];
    int par2cld[2];
    checkError(pipe(cld2par)!=-1,"");
    checkError(pipe(par2cld)!=-1,"");
    pid_t childPid = fork();
    checkError(childPid!=-1,"");
    if(childPid!=0){ //parent process
        close(cld2par[WR]); //dete hvata info i cita -> RD
        close(par2cld[RD]); //roditelj upisuje -> WR

        //dakle, ovde nam je trivijalno da ce na rd kraj da cita a na wr kraj da pise
        checkError(write(par2cld[WR],path,strlen(path))!=-1,"");
        char buff[MAX];

        //ceka roditelj da mu dete odgovori, dete cita info i prosledice roditelju
        int readBytes = read(cld2par[RD],buff,MAX-1);
        checkError(readBytes!=-1,"");
        buff[readBytes]='\0';

        int num=0;
        sscanf(buff,"%d",&num);
        numLines+=num;

        close(cld2par[RD]);
        close(par2cld[WR]);

        int status = 0;
        checkError(wait(&status)!=-1,"");

    }else{ //child process
        close(cld2par[RD]); //dakle dete odredjuje br linija i salje roditelju -> WR
        close(par2cld[WR]); //a ovde roditelj hvata info i cita -> RD

        char pathtmp[MAX];
        //svaki put citaj od roditelja ka detetu sa read kraja komandu MAXSIZE bajtova

        //i ovde nam je trivijalno da ce na rd kraj da cita a na wr kraj da pise
        int readBytes = read(par2cld[RD],pathtmp,MAX-1);
        checkError(readBytes!=-1,"");
        pathtmp[readBytes]='\0';

        //dete salje roditelju info o bajtovima i preusmerava info na standardni izlaz
        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"");
         //brojimo linije sa nase tmp putanje
        checkError(execlp("wc","wc","-l",pathtmp,(char*)NULL)!=-1,"");

        close(cld2par[WR]);
        close(par2cld[RD]);
    }
    wait(NULL);
    printf("%d\n",numLines);
    free(path);
    exit(EXIT_SUCCESS);
}