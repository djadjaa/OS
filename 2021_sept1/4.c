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

#define RD 0
#define WR 1
#define MAX_LINE 255
#define MAX_COM 4094

int main(int argc, char **argv){

    checkError(argc==2,"args");
    //jedan nacin je preko getline-a (2020_jan1 zad)
    //drugi nacin:
    FILE *f = fopen(argv[1],"r");
    FILE *foutput = fopen("errors.txt","w"); //u ovaj fajl upisujemo greske 
    checkError(f!=NULL,"fopen");
    char command[MAX_LINE]={0};
    char argument[MAX_LINE] = {0};
    while(fscanf(f,"%s%s",command,argument)!=EOF){
        char res[MAX_COM];
        int cld2par[2];
        checkError(pipe(cld2par)!=-1,"pipe");
        pid_t childPid = fork();
        checkError(childPid!=-1,"fork");
        if(childPid==0){
            close(cld2par[RD]);
            checkError(dup2(cld2par[WR],STDERR_FILENO)!=-1,"dup2");
            checkError(execlp(command,command,argument,(char*)NULL)!=-1,"execlp");
            exit(EXIT_SUCCESS);
        }else{
            close(cld2par[WR]);
            //pokupiće svaki karakter sa kraja za čitanje i smestiće ga u buffer
            FILE *f_err = fdopen(cld2par[RD],"r");
            checkError(f_err!=NULL,"fdopen");

            int i=0;
            while((res[i]=getc(f_err))!=EOF) //svaki karakter sa f_err fda ubacujemo u nas bafer
                i++;
            res[i]='\0';


            fclose(f_err);
        }
        int status = 0;
        checkError(wait(&status)!=-1,"wait");
        if(WIFEXITED(status)){
            if(WEXITSTATUS(status)!=EXIT_SUCCESS){
                fprintf(foutput,"%s",res); //ubacujemo u errors.txt
            }
        }
    }
    fclose(foutput);
    fclose(f);
    exit(EXIT_SUCCESS);
}