#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>
#include <semaphore.h>
#include <sys/mman.h>

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 256

typedef struct{
    sem_t inDataReady;
    sem_t outDataReady;
    char s[MAX];
}OsInputData;

void *createMemoryBlock(const char *fpath, unsigned size){
    int fd = shm_open(fpath,O_RDWR|O_CREAT,0600);
    checkError(fd!=-1,"");
    checkError(ftruncate(fd,size)!=-1,"");
    void *addr = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"");
    close(fd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==3,"");
    int n = atoi(argv[2]);
    OsInputData *sharedMem=createMemoryBlock(argv[1],sizeof(OsInputData));
    checkError(sem_init(&sharedMem->inDataReady,1,1)!=-1,""); //writer
    checkError(sem_init(&sharedMem->outDataReady,1,0)!=-1,""); //reader
    scanf("%s",sharedMem->s);
    checkError(sem_post(&sharedMem->inDataReady)!=-1,""); //zbog cekanja inDataReady semafora
    checkError(munmap(sharedMem,sizeof(sharedMem))!=-1,"");
    
    exit(EXIT_SUCCESS);
}