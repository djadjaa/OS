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
    checkError(argc==4,"args");
    char *fPath=argv[1];
    char *wrd=argv[2];
    char *rplc=argv[3];
    int fd=open(fPath,O_RDWR);
    checkError(fd!=-1,"open");
    //citamo rec po rec i menjamo koja treba
    FILE *f=fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");
    char temp[MAX];
    int br=0;
    while(fscanf(f,"%s",temp)==1){
        if(strcmp(temp,wrd)==0){
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET; //koristimo fajl strim, zato koristimo ovo i narednu naredbu
            lock.l_start = (int)ftell(f)-strlen(temp); //pozicija sa ftell
            lock.l_len = strlen(temp);
            int retVal=fcntl(fd,F_SETLK,&lock);
            /*
             ako je rec zakljucana ona je neizmenjena pa uvecavamo brojac, u suprotnom je menjamo pa zakljucavamo
             */
            if(retVal==-1){ //neko je zakljucao
                if(errno !=EACCES && errno!=EAGAIN){
                    checkError(0,"...");
                }else{ //posto je zakljucana a neizmenjena uvecavamo brojac sa nepromenjenim recima,otkljucavamo je i idemo dalje
                    br++;
                    lock.l_type = F_UNLCK;
                    checkError(fcntl(fd,F_SETLK,&lock)!=-1,"...");
                }
            }else{ //nije zakljucana pa je menjamo pa cemo je ponovo zakljucati
                checkError(fseek(f,-strlen(temp),SEEK_CUR)!=-1,"fseek");
                fprintf(f,"%s",rplc);
                checkError(fcntl(fd,F_SETLK,&lock)!=-1,"fcntl");
            }
        }
    }
    checkError(feof(f),"eof");
    printf("%d\n",br);
    fclose(f);
    exit(EXIT_SUCCESS);
}
