#define _XOPEN_SOURCE 700
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
    }while(0); \

#define RD 0
#define WR 1
#define NUMCOM 256

char **parse(char *line){
    //alokacija niza argumenata
    char **args = malloc(NUMCOM*sizeof(char*));
    checkError(args!=NULL,"");
    int i=0;
    char *token = strtok(line," \t\n");
    //popunjavanje niza argumenata tokenima koje razdvajamo belinama
    while(token!=NULL && i<NUMCOM-1){
        args[i] = strdup(token); //malloc+strcpy
        checkError(args[i]!=NULL,"");
        i++;
        //idemo dalje
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
    return args;
}

void freeargs(char **args){
    for(int i=0; args[i]!=NULL; i++)
        free(args[i]);
    free(args);
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    FILE *f = fopen(argv[1],"r");
    checkError(f!=NULL,"");

    char *line=NULL;
    size_t len=0;
    char komandares[NUMCOM] = "unknown";
    int resnum=0;

    while(getline(&line,&len,f)>0){
        char **args = parse(line);
        if(args[0]==NULL){
            freeargs(args);
            continue;
        }

        /*
        DRUGI NACIN!!! (ako i samo ako imamo unapred poznat broj komandi sto ovde kod nas nije slucaj)
        int n;
        int ukprocitano=0, procitano=0;
        //iz linije izvlacimo broj n i koliko je bajtova procitano za sad do broja n; zanemarujemo razmak
        sscanf(line+ukprocitano,"%d%*c%n", &n,&procitano);
        ukprocitano+=procitano;
        char **args = (char**)malloc((n+1)*sizeof(char*));
        checkError(args!=NULL,"malloc");
        for(int i=0; i<n; i++){
            args[i] = (char*)malloc(MAX*sizeof(char));
            checkError(args[i]!=NULL,"malloc args[i]");
        }
        //izvlacimo argumente i koliko je bajtova procitano, opet zanemarujemo razmake
        for(int i=0; i<n; i++){
            sscanf(line+ukprocitano,"%s%*c%n", args[i],&procitano);
            ukprocitano+=procitano;
        }
        args[n] = NULL;
        */

        int cld2par[2];
        checkError(pipe(cld2par)!=-1,"");
        pid_t childPid = fork();
        checkError(childPid!=-1,"");
        if(childPid==0){
            close(cld2par[RD]);

            //preusmeravanje standardnog izlaza na dev/null

            int devnullFd = open("/dev/null", O_WRONLY);
            checkError(devnullFd!=-1,"");
            checkError(dup2(devnullFd,STDOUT_FILENO)!=-1,"");
            
            close(devnullFd);

            //preusmeravamo dete na izlaz za gresku
            checkError(dup2(cld2par[WR],STDERR_FILENO)!=-1,"");
            checkError(execvp(args[0],args)!=-1,"");

            close(cld2par[WR]);
            exit(EXIT_FAILURE);
        }else{
            close(cld2par[WR]);

            int status=0;
            checkError(wait(&status)!=-1,"");
            if(!WIFEXITED(status) || WEXITSTATUS(status)!=EXIT_SUCCESS){
                int totalchar = 0;
                char buff[NUMCOM];
                int readBytes=0;
                while((readBytes=read(cld2par[RD],buff,NUMCOM))>0)
                    totalchar+=readBytes;
                checkError(readBytes!=-1,"");
                if(totalchar > resnum){
                    resnum = totalchar;
                    strncpy(komandares,args[0],sizeof(komandares)-1);
                    komandares[sizeof(komandares)-1]='\0';
                }
            }
            close(cld2par[RD]);
            freeargs(args);
        }
        free(line);
    }
    printf("%s %d\n",komandares,resnum);
    fclose(f);
    exit(EXIT_SUCCESS);
}
