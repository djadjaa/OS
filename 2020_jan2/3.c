#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 2
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"Open");
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");

    char buff[MAX];
    int numUnchanged=0;

    while(fscanf(f,"%s",buff)==1){
        if(strcmp(buff,argv[2])==0){
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = ftell(f);
            lock.l_len = -strlen(buff);
            
            //ako je rec zakljucana ona je neizmenjena pa uvecavamo brojac, u suprotnom je menjamo pa zakljucavamo

            checkError(fcntl(fd,F_GETLK,&lock)!=-1,"fcntl");
            if(lock.l_type!=F_UNLCK){
                numUnchanged++;
            }
            else{
                checkError(fseek(f,-strlen(buff),SEEK_CUR)!=-1,"Fseek");
                fprintf(f,"%s",argv[3]);
                checkError(fcntl(fd,F_SETLK,&lock)!=-1,"fcntl");
            }
        }
    }
    printf("%d\n",numUnchanged);
    fclose(f);
    exit(EXIT_SUCCESS);
}
