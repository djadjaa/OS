#define _POSIX_C_SOURCE 2
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define WORD 256
#define MAX 1024

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int fd=open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");
    //citamo rec po rec
    FILE *f=fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");
    char buffer[MAX];
    int br=0;
    while(fscanf(f,"%s",buffer)==1){
        char *stringError=NULL;
        int retVal=strtol(buffer,&stringError,10);
        if(buffer!=NULL && *stringError=='\0'){ //tada je broj i pokusacemo da ga zakljucamo
            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET; //koristimo fajl strim, zato koristimo ovo i narednu naredbu
            lock.l_start = ftell(f); //pozicija sa ftell
            lock.l_len = -strlen(buffer); //zakljucavamo unazad
            retVal=fcntl(fd,F_SETLK,&lock);
            if(retVal==-1){
                if(errno !=EACCES && errno!=EAGAIN){ //ako neki drugi proces ne drzi katanac
                    checkError(0,"...");
                }
            }else{
                br++;
                lock.l_type=F_UNLCK;
                checkError(fcntl(fd,F_SETLK,&lock)!=-1,"fcntl");
            }
        }
    }
    checkError(feof(f),"eof");//proveravamo da li smo stigli do kraja fajla
    fclose(f);
    printf("%d\n",br);
    exit(EXIT_SUCCESS);
}
