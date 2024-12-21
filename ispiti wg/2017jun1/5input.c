#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <math.h>
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
    float array[ARRAY_MAX];
    unsigned arrayLen;
}OsInputData;

void *getMemBlock(const char *path,int *size){
    //int shm_open(const char *name, int oflag, mode_t mode);
    int memFd = shm_open(path,O_RDWR,0);
    checkError(memFd!=-1,"shm_open");
    struct stat fInfo;
    checkError(fstat(memFd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;
    //void *mmap(void *addr, size_t length, int prot, int flags,int fd, off_t offset);
    void *addr = mmap(NULL,*size,PROT_READ | PROT_WRITE, MAP_SHARED, memFd,0);
    checkError(addr!=MAP_FAILED, "mmap");
    close(memFd);
    return addr;
}

/*
 devijacija niza je sqrt(1/N*suma(xi-aritmsr)²)
 gde suma krece od prvog do Ntog clana
 */

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int size = 0;
    OsInputData *memData = (OsInputData*)getMemBlock(argv[1],&size);
    checkError(sem_wait(&(memData->inDataReady))!=-1,"sem_wait");

    float sum = 0.0f,as;
    unsigned int arraySize = memData->arrayLen;
    for(int i=0; i<arraySize; i++){
        sum += memData->array[i];
    }
    as = sum/arraySize; //aritmeticka sredina
    sum = 0.0f; //restartujemo vr

    for(int i=0; i<arraySize; i++){
        sum += (memData->array[i]-as)*(memData->array[i]-as); //+ kvadriramo vrednost paralelno
    }

    float rez=sum/arraySize;
    rez = sqrtf(rez);

    printf("%f\n",rez);

    checkError(munmap(memData,sizeof(memData))!=-1,"munmap");
    exit(EXIT_SUCCESS);
}
