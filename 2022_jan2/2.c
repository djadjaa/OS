#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
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
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISREG(fInfo.st_mode),"not regular");
    char *ext = strrchr(argv[1],'.');
    if(ext!=NULL & strcmp(ext,".c")!=0)
        checkError(0,"not c file");
    int n = strlen(argv[1]);
    char *assembly = (char*)malloc(n*sizeof(char));
    checkError(assembly!=NULL,"malloc");
    strcpy(assembly,argv[1]);
    assembly[n-1]='s'; //promena ekstenzije;
    //printf("%s %s\n",argv[1],assembly);

    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"pipe");
    pid_t childPid = fork();
    checkError(childPid!=-1,"fork");

    if(childPid==0){
        close(cld2par[RD]);
        checkError(dup2(cld2par[WR],STDOUT_FILENO)!=-1,"dup2");
        checkError(execlp("gcc","gcc","-std=c99","-S",argv[1],"-o",assembly,(char*)NULL)!=-1,"execlp");

    }else{
        close(cld2par[RD]);
        int status = 0;
        checkError(wait(&status)!=-1,"wait");

        if(WIFEXITED(status)){
            if(WEXITSTATUS(status)==EXIT_SUCCESS){

                FILE *f = fopen(assembly,"r");
                checkError(f!=NULL,"fopen");

                char *line=NULL;
                size_t len=0;
                int numMovKom=0;

                while(getline(&line,&len,f)>0){
                    if(strstr(line,"mov")!=NULL)
                        numMovKom++;
                }
                fprintf(stdout,"%d\n",numMovKom);
                free(line);
                fclose(f);
            }else{
                free(assembly);
                exit(WEXITSTATUS(status));
            }
        }else{
            free(assembly);
            checkError(0,"exit");
        }
        close(cld2par[RD]);
    }
    checkError(unlink(assembly)!=-1,"deleting");
    free(assembly);
    exit(EXIT_SUCCESS);
}