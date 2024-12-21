#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
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

int main(int argc, char **argv){

    checkError(argc>=2,"args");
    int maxLines=0;
    for(int i=1; i<argc; i++){
        //PROVERA DA LI JE PUTANJA DOBRA !!! #TODO
        //char *path = basename(argv[i]);
       // printf("%s\n",path);
        char *path = argv[i];
        int cld2par[2];
        checkError(pipe(cld2par)!=-1,"pipe");
        pid_t childPid = fork();
        checkError(childPid!=-1,"fork");
        if(childPid==0){
            close(cld2par[RD]);
            checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
            checkError(execlp("cat","cat",path,(char*)NULL)!=-1,"execlp");
            exit(EXIT_SUCCESS);
        }else{
            close(cld2par[WR]);
            FILE *f = fdopen(cld2par[RD],"r");
            checkError(f!=NULL,"fdopen");
            
            char *line=NULL;
            size_t len=0;
            int numLines=0;
            while(getline(&line,&len,f)>0){
                numLines++;

                if(numLines>maxLines)
                    maxLines = numLines;

                free(line);
            }
            fclose(f);
        }
    }
    int status=0;
    checkError(wait(&status)!=-1,"wait");
    if(!(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS))
        printf("neuspeh\n");
    else
        printf("%d\n",maxLines);
    
    exit(EXIT_SUCCESS);
}