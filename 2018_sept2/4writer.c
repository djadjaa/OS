#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
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

void *createMemoryBlock(const char *fpath, unsigned size){
    int fd = shm_open(fpath,O_RDWR|O_CREAT,0600);
    checkError(fd!=-1,"");
    checkError(ftruncate(fd,size)!=-1,"");
    void *addr = mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"");
    close(fd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    shm_obj *sharedMemory = createMemoryBlock(argv[1],sizeof(shm_obj));
    checkError(sem_init(&sharedMemory->inDataReady,1,1)!=-1,"");
    scanf("%u",&sharedMemory->arrayLen);
    for(int i=0; i<sharedMemory->arrayLen; i++)
        scanf("%d",&sharedMemory->a[i]);
    checkError(sem_post(&sharedMemory->inDataReady)!=-1,"");
    checkError(munmap(sharedMemory,sizeof(shm_obj))!=-1,"");
    exit(EXIT_SUCCESS);
}
