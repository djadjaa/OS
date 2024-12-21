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

#define MAX 256

int main(int argc, char **argv){

    checkError(argc==2,"args");
    char *fPath=argv[1];
    int fd=open(fPath,O_RDWR);
    checkError(fd!=-1,"open");
    FILE *f=fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");
    char tmp[MAX];
    while(fscanf(f,"%s",tmp)==1){
        int num=strtol(tmp,NULL,10);
        if(num>=1000 && num<=9999){
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET; //koristimo fajl strim, zato koristimo ovo i narednu naredbu
            lock.l_start = (int)ftell(f)-strlen(tmp); //pozicija sa ftell
            lock.l_len = strlen(tmp);
            int retVal=fcntl(fd,F_SETLK,&lock);
            if(retVal==-1){ //neko je zakljucao
                 if(errno !=EACCES && errno!=EAGAIN){
                    checkError(0,"...");
                }else{ //ako je zakljucano samo otkljucavamo i idemo dalje
                    lock.l_type = F_UNLCK;
                    checkError(fcntl(fd,F_SETLK,&lock)!=-1,"...");
                }
            }else{ //nije zakljucan broj pa menjamo pa zakljucavamo
                checkError(fseek(f,-strlen(tmp),SEEK_CUR)!=-1,"fseek");
                fprintf(f,"####");
                checkError(fcntl(fd,F_SETLK,&lock)!=-1,"fcntl");
            }
        }
    }
    checkError(feof(f),"eof");
    fclose(f);
    exit(EXIT_SUCCESS);
}
