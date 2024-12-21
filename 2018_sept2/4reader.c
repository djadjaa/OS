#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <limits.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define ARRAY_MAX 1024

typedef struct {
    sem_t inDataReady;
    unsigned arrayLen;
    int a[ARRAY_MAX];
}shm_obj;

void *getMemoryBlock(const char *fpath, unsigned *size){
    int fd = shm_open(fpath,O_RDWR|O_CREAT,0600);
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

    checkError(argc==2,"");
    unsigned size=0;
    shm_obj *sharedMemory = getMemoryBlock(argv[1],&size);
    checkError(sem_wait(&sharedMemory->inDataReady)!=-1,"");

    int max=INT_MIN;
    for(int i=0; i<sharedMemory->arrayLen; i++){
        if(sharedMemory->a[i]>max)
            max=sharedMemory->a[i];
    }
    printf("%d\n",max);
    checkError(sem_post(&sharedMemory->inDataReady)!=-1,"");
    checkError(munmap(sharedMemory,size)!=-1,"");
    exit(EXIT_SUCCESS);
}
