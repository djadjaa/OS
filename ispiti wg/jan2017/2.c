#define _POSIX_C_SOURCE 200809L
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0);

#define RD_END (0)
#define WR_END (1)
#define MAXSIZE (1024)

int main(int argc, char **argv){

    checkError(argc==2,"args");

    int cld2par[2];
    checkError(pipe(cld2par)!=-1,"child 2 parent");
    /*
    int par2cld[2];
    checkError(pipe(par2cld)!=-1,"parent 2 child");
    */
    pid_t childPid=fork();
    checkError(childPid!=-1,"child process");

    if(childPid==0){ //child process
        close(cld2par[RD_END]);
        checkError(dup2(cld2par[WR_END],STDOUT_FILENO)!=-1,"dup2"); //kopiramo pajp u stdout fd
        checkError(execlp("stat", "stat","--format=%s" ,argv[1], (char*)NULL)!=-1, "execlp"); //izvrsava se preko dete procesa program stat
        exit(EXIT_SUCCESS);
    }else{ //parent process
        close(cld2par[WR_END]);
        FILE *fd=fdopen(cld2par[RD_END],"r"); //otvaramo pajp sa read kraja, tacnije duplirani fajl deskriptor
        checkError(fd!=NULL,"fd");
        size_t readBytes=0;
        char *line=NULL;
        int x=getline(&line,&readBytes,fd); //citamo liniju (liniju gde se prikazuje velicina fajla)
        int status=0;
        checkError(wait(&status)!=-1,"...");
        if(!(WIFEXITED(status) && WEXITSTATUS(status)==EXIT_SUCCESS) && x==-1){
            printf("Neuspeh\n");
            exit(EXIT_SUCCESS); // <=> exit(0)
        }
        int size=strtol(line,NULL,10); //konvertujemo u broj
        printf("%d\n",size);
        free(line); //oslobodimo sve sto treba
        fclose(fd);
    }

    exit(EXIT_SUCCESS);
}
