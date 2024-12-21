#define _POSIX_C_SOURCE 2
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX 256

#define checkError(expr,userMsg) \
    do{\
        if(!(expr)){\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)\

int main(int argc, char **argv){

    checkError(argc==3,"args");
    char *fPath=argv[1];
    int fd=open(fPath,O_RDWR);
    checkError(fd!=-1,"open");
    FILE *f=fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");
    char wrd[MAX];
    struct flock lock;
    while(fscanf(f,"%s",wrd)==1){
        if(strcmp(wrd,argv[2])==0){
            lock.l_type=F_WRLCK;
            lock.l_whence=SEEK_SET;//fajl strim
            lock.l_start=ftell(f);//tekuca pozicija
            lock.l_len=-strlen(wrd);//zakljucavamo od pozadi do prvog bajta reci
            int retVal=fcntl(fd,F_GETLK,&lock);
            if(retVal!=-1){
                if(lock.l_type==F_WRLCK){
                    printf("%ld w\n",ftell(f)-strlen(wrd));
                }else if(lock.l_type==F_RDLCK){
                    printf("%ld r\n",ftell(f)-strlen(wrd));
                }
            }else{
                if(errno!=EACCES && errno!= EAGAIN){
                    checkError(0,"...");
                }
            }
        }
    }
    checkError(feof(f),"eof");
    fclose(f);
    exit(EXIT_SUCCESS);
}
