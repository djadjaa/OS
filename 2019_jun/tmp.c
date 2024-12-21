#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

#define checkError(expr,usrMsg) \
    do { \
        if(!(expr)){ \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0); \

#define WR_END (1)
#define RD_END (0)
int main(int argc, char **argv){

    checkError(argc==2, "args");
    FILE *f=fopen(argv[1],"r");
    checkError(f!=NULL,"fopen");
    size_t len=0;
    int numChild=0,numA=0;
    char *line=NULL;
    /*ovo radimo da bi i dete i roditelj proces imali pristup promenljivoj, u suprotnom ne bi; naravno preko forka će zvanićno dinamički alocirana promenljva biti dostupna kako jednom tako drugom procesu

    int *numA = malloc(sizeof(int));
     //odvojiće nam dovoljno bajta da smestimo rezultat
    checkError(numA!=NULL,"malloc");
    */
    while(getline(&line,&len,f)!=-1){
        int pajp[2];
        checkError(pipe(pajp)!=-1,"pipe");
        pid_t childPid = fork();
        checkError(childPid!=-1,"fork");
        if(childPid==0){ //child process
            close(pajp[RD_END]);
            //checkError(dup2(pajp[WR_END],STDOUT_FILENO)!=-1,"dup2");
            int num=0;
            for(int i=0; line[i]!='\0'; i++){
                if(line[i]=='a')
                    num++;
            }
            //*numA += num;
            int *buff = malloc(sizeof(int));
            checkError(buff!=NULL,"malloc");
            *buff=num;
            checkError(write(pajp[WR_END],buff,sizeof(int))!=-1,"write");
            //write fji dajemo fajl deskriptor u koji pišemo podatke, bafer čiji će sadržaj da upiše i veličinu bafera
            //bafer upisuje u pajp[wrend] jer čitamo iz bafera
            free(buff);
            _exit(EXIT_SUCCESS);
        }else{ //parent process
            close(pajp[WR_END]);
            numChild++;
            int *buff=malloc(sizeof(int));
            checkError(buff!=NULL,"malloc");
            checkError(read(pajp[RD_END],buff,sizeof(int))!=-1,"read");
            //read fji prosleđujemo fajl deskriptor iz kog dobija podatke/sadržaj, bafer u koji upisujemo podatke koji su pročitani i veličinu bafera
            //pajp[rdend] upisuje u bafer jer čitamo iz pajpa
            //read dobija bajtove od fajl deskriptora koji smo joj prosledili i od tog pokazivača upisuje u bafer bajt po bajt
            //malloc će nam rezervisati memoriju isključivo za bafer koja ćese koristiti baš isključivo za bafer, u suprotnom bismo imali ili neinicijalizovani pokazivač koji bi imao neku nasumičnu vrednost u tom slučaju, ili bismo imali inicijalizovan na NULL što nam predstavlja problem jer nema gde da piše bajtove iz fajla
            numA += *buff; //dodajemo vrednost koju smo pročitali
            int status=0;
            checkError(wait(&status)!=-1,"wait");
            free(buff);
        }
        line=NULL;
    }
    fclose(f);
    free(line);
    printf("%d %d\n", numA, numChild);
    exit(EXIT_SUCCESS);
}