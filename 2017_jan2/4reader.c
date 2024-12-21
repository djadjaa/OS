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

void *getMemoryBlock(const char *fpath, unsigned *size){
    int fd = shm_open(fpath,O_RDWR,0);
    checkError(fd!=-1,"");

    struct stat fInfo;
    checkError(fstat(fd,&fInfo)!=-1,"");
    *size = fInfo.st_size;

    void *addr = mmap(NULL,*size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"");
    close(fd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==3,"");
    int n = atoi(argv[2]);
    unsigned size=0;
    OsInputData *sharedMem=getMemoryBlock(argv[1],&size);

    checkError(sem_wait(&sharedMem->inDataReady)!=-1,"");

    for (int i = 0; sharedMem->s[i] != 0; i++) {
		sharedMem->s[i] = (sharedMem->s[i] + n) % 128; 
	}
    printf("%s\n",sharedMem->s);
    checkError(sem_post(&sharedMem->outDataReady)!=-1,""); //zbog cekanja inDataReady semafora
    checkError(munmap(sharedMem,size)!=-1,"");
    
    exit(EXIT_SUCCESS);
}