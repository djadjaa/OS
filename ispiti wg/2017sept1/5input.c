#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <math.h>
#include <stdbool.h>
#include <semaphore.h>

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
    int array[ARRAY_MAX];
    unsigned arrayLen;
}OsInputData;

void *getMemBlock(const char *fPath,int *size){
    //shm_open(const char *name, int oflag, mode_t mode);
    int memFd = shm_open(fPath, O_RDWR, 0);
    checkError(memFd!=-1,"shm_open");
    struct stat fInfo;
    checkError(fstat(memFd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;
    void *addr = mmap(NULL,*size,PROT_READ|PROT_WRITE,MAP_SHARED,memFd,0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(memFd);
    return addr;
}

bool stepenTrojke(int x){
    if(x<=0)
        return false;
    if(x%3==0)
        return stepenTrojke(x/3);
    if(x==1)
        return true;
    return false;
}

int main(int argc, char **argv){
    checkError(argc==2,"args");
    int size = 0;
    OsInputData *memData = (OsInputData*) getMemBlock(argv[1],&size);
    checkError(sem_wait(&(memData->inDataReady))!=-1,"sem_wait");
    unsigned arrSize = memData->arrayLen;
    for(int i=0; i<arrSize; i++){
        if(stepenTrojke(memData->array[i])){
            printf("%d ",memData->array[i]);
        }
    }
    checkError(munmap(memData,sizeof(memData))!=-1,"munmap");
    return 0;
}
