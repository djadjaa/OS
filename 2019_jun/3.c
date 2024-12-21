#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define RD 0
#define WR 1
int main(int argc, char **argv){

    checkError(argc==2,"args");
    FILE *f = fopen(argv[1],"r");
    checkError(f!=NULL,"fopen");
    int ukBrPr=0,ukBrA=0;
    
    char *line=NULL;
    size_t len;
    while(getline(&line,&len,f)!=-1){
        int cld2par[2];
        checkError(pipe(cld2par)!=-1,"pipe");
        pid_t childPid = fork();
        checkError(childPid!=-1,"fork");
        if(childPid==0){
            close(cld2par[RD]);
            int numtmp=0;
            for(int i=0; line[i]!='\0'; i++){
                if(line[i]=='a')
                    numtmp++;
            }
            
            int *buff = malloc(sizeof(int));
            *buff=numtmp;
            checkError(write(cld2par[WR],buff,sizeof(int))!=-1,"write");
            
            //*ukBrA += numtmp;
            close(cld2par[WR]);
            free(buff);
            _exit(EXIT_SUCCESS);
        }else{
            close(cld2par[WR]);
            ukBrPr++;
            //dete proces direktno upisuje br u memoriju tamo gde treba a na nama je samo da prebrojimo br dete procesa

            int *buff=malloc(sizeof(int));
            checkError(read(cld2par[RD],buff,sizeof(int))!=-1,"read");
            ukBrA += *buff;

            int status=0;
            checkError(wait(&status)!=-1,"wait");

            close(cld2par[RD]);
            free(buff);
        }
        line=NULL;
    }

    fclose(f);
    free(line);
    printf("%d %d\n",ukBrA,ukBrPr);
    exit(EXIT_SUCCESS);
}