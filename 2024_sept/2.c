#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <string.h>
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
#define MAX 256

int main(int argc, char **argv){
    checkError(argc==2,"args");
    /*
    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"pipe");*/
    pid_t childPid = fork();
    checkError(childPid!=-1,"fork");

    int len = strlen(argv[1]);
    checkError(strcmp(argv[1]+len-2,".c")==0,"");
    char asmpath[MAX];
    strncpy(asmpath,argv[1],len-2);
    asmpath[len-2]='\0';
    strcat(asmpath,".s");

    int nummov=0;

    //direktno cemo pokrenuti execlp iz dete procesa, nije nam neophodan pajp
    if(childPid==0){
        //close(cld2par[RD]);

        //checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("gcc","gcc","-std=c99", "-S", argv[1],"-o",asmpath,(char*)NULL)!=-1,"execlp");

        //close(cld2par[WR]);
    }else{
        //close(cld2par[WR]);

        //FILE *f = fdopen(cld2par[RD],"r");
        FILE *f = fopen(asmpath,"r");
        checkError(f!=NULL,"fdopen");

        char *line=NULL;
        size_t len=0;
        while (getline(&line, &len, f) != -1) {
            //formiramo niz reci
            char *token = strtok(line, " \t\n");
            while (token != NULL) {
                if (strncmp(token, "mov",3) == 0) {
                    nummov++;
                }
                token = strtok(NULL, " \t\n");
            }
        }
        
        free(line);
        fclose(f);
        //close(cld2par[RD]);

        printf("%d\n",nummov);
        int status=0;
        checkError(wait(&status)!=-1,"wait");
    }

    exit(EXIT_SUCCESS);
}
