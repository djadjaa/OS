#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ftw.h>
#include <errno.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 1024

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int fd=open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");
    FILE *f= fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");
    char buff[MAX];
    int numChar=0;

    while(fscanf(f,"%s",buff)==1){
        char *err=NULL;
        int num = strtol(buff,&err,10);
        if(buff[0]!='\0' && *err=='\0'){ //ako nismo stigli do novog reda
            struct flock lock; //info o zakljucavanju
            lock.l_type = F_WRLCK;
            lock.l_start = ftell(f); //zakljucavamo u odnosu na poslednju procitanu poziciju pomocu fja iz stdio biblioteke
            lock.l_len = -strlen(buff); //uvek se zakljucava unazad relativno u odnosu na trenutni polozaj jer je potrebno zameniti rec sa ##..#
            lock.l_whence = SEEK_SET; //uvek posmatramo od pocetka
            int retVal = fcntl(fd,F_SETLK,&lock); //postavljanje katanca
            if(retVal != -1){
                for(int i=0; buff[i]!='\0'; i++)
                    buff[i]='#';
                fseek(f,-strlen(buff),SEEK_CUR);
                fprintf(f,"%s",buff);

                numChar+=strlen(buff);

                lock.l_type = F_UNLCK;
                checkError(fcntl(fd,F_SETLK,&lock)!=-1,"fcntl");
            }
        }else{
            if(errno!=EACCES && errno != EAGAIN)
                checkError(0,"errno");
            continue;
        }
    }
    printf("%d\n",numChar);
    fclose(f);
    
    exit(EXIT_SUCCESS);
}