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
    float array[ARRAY_MAX];
    unsigned arrayLen;
}osData;

void *createMemoryBlock(const char *path, unsigned int size){
    int fd = shm_open(path, O_RDWR|O_CREAT, 0600);
    checkError(fd!=-1,"open");
    checkError(ftruncate(fd,size)!=-1,"ftruncate");
    void *addr = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"Mmap");
    close(fd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    osData *sharedObj = createMemoryBlock(argv[1],sizeof(osData));
    checkError(sem_init(&sharedObj->inDataReady,1,1)!=-1,"sem init");
    scanf("%u",&sharedObj->arrayLen);
    for(int i=0; i<sharedObj->arrayLen; i++)
        scanf("%f",&sharedObj->array[i]);
    checkError(sem_post(&sharedObj->inDataReady)!=-1,"sem post");
    checkError(munmap(sharedObj,sizeof(osData))!=-1,"Munmap");
    exit(EXIT_SUCCESS);
}