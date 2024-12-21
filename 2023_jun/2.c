#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define checkError(expr,usrMsg) \
    do{ \
        if (!(expr)){ \
            perror(usrMsg); \
            exit(1); \
        } \
    }while(0)

#define RD 0
#define WR 1
#define MAXSIZE 4096
int main(int argc, char **argv){

    checkError(argc==1,"args");
    int numLines=0;
    char path[MAXSIZE];
    while(scanf("%s",path)==1){
        int cld2par[2];
        int par2cld[2];
        checkError(pipe(cld2par)!=-1,"pipe cld2par");
        checkError(pipe(par2cld)!=-1,"pipe par2cld");

        pid_t childPid = fork();
        checkError(childPid!=-1,"fork");

        if(childPid!=0){ //parent process
        //roditelj ce da ispise sve sto treba a dete ce da procita
            close(cld2par[WR]);
            close(par2cld[RD]);

            checkError(write(par2cld[WR],path,strlen(path))!=-1,"write"); //roditelj prebacuje ime fajla na pajp

            char buff[MAXSIZE];
            //ceka roditelj da mu dete odg
            int readBytes= read(cld2par[RD],buff,MAXSIZE-1); //roditelj cita iz pajpa u buffer
            checkError(readBytes!=-1,"read");
            buff[readBytes]='\0';

            int num=0;
            sscanf(buff,"%d",&num); //u bafer se upisuje rezultat komande wc
            numLines+=num;

            close(cld2par[RD]);
            close(par2cld[WR]);
            //proveravamo sta je dete odg
            int status=0;
            checkError(wait(&status)!=-1,"wait");
        }else{ //child process
        //roditelj ce da procita sve sto treba a dete ce da ispise
        //odnosno dete ce da ispise sve sto treba a roditelj ce da procita
            close(cld2par[RD]);
            close(par2cld[WR]);

            char pathtmp[MAXSIZE];
            //svaki put citaj od roditelja ka detetu sa read kraja komandu MAXSIZE bajtova
            int readBytes = read(par2cld[RD],pathtmp,MAXSIZE-1); //dete cita iz pajpa u nov naziv za putanju
            checkError(readBytes!=-1,"read");
            pathtmp[readBytes]='\0';
            //posalji roditelju da roditelj barata
            checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"DUP2"); //dete preusmerava standardni izlaz na roditelja
            checkError(execlp("wc","wc","-l",pathtmp,(char*)NULL)!=-1,"execlp"); //brojimo linije sa nase tmp putanje

            close(cld2par[WR]);
            close(par2cld[RD]);
        }
    }
    wait(NULL); //da sprecimo prepunjavanje sistema procesima koji su ubijeni a nisu sacekani
    printf("%d\n",numLines);
    exit(EXIT_SUCCESS);
}