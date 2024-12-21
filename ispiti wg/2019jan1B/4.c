#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define checkError(expr,userMsg) \
    do{\
        if(!(expr)){\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\

#define MAX_COM 4094
#define MAX_LINE 255
#define WR_END 1
#define RD_END 0
int main(int argc, char **argv){

    checkError(argc==2,"args");
    FILE *f = fopen(argv[1],"r");
    FILE *output = fopen("errors.txt","w");
    checkError(f!=NULL,"fopen");
    char komanda[MAX_LINE]={0};
    char argument[MAX_LINE]={0};
    while(fscanf(f,"%s%s",komanda,argument)!=EOF){
        //fscanf(f,"%s",argument);
        //printf("%s %s\n",komanda,argument);
        int pajp[2];
        checkError(pipe(pajp)!=-1,"pipe");
        pid_t childPid = fork();
        checkError(childPid!=-1,"fork");
        char resBuff[MAX_COM+1];
        if(childPid==0){
            close(pajp[RD_END]);

            checkError(dup2(pajp[WR_END],STDERR_FILENO)!=-1,"dup2");
            checkError(execlp(komanda,komanda,argument,(char*)NULL)!=-1,"execlp");

            exit(EXIT_SUCCESS);
        }else{
            close(pajp[WR_END]);

            /*
             * DRUGI NACIN
            int chRead=0;
            checkError((chRead=read(pajp[RD_END],&resBuff,MAX_COM+1))!=-1,"read");
            resBuff[chRead]='\0';

            exit(EXIT_SUCCESS);
            */
            //pokupiće svaki karakter sa kraja za čitanje i smestiće ga u buffer
            FILE* f_err = fdopen(pajp[RD_END], "r");

            int i = 0;
            while((resBuff[i] = getc(f_err)) != EOF){
                i++;
            }
            resBuff[i] = '\0';

            fclose(f_err);
        }
        int status=0;
        checkError(wait(&status)!=-1,"wait");
        if(WIFEXITED(status)){
            if(WEXITSTATUS(status) != 0)
                fprintf(output, "%s", resBuff);
        }
    }

    fclose(output);
    fclose(f);
    exit(EXIT_SUCCESS);
}
