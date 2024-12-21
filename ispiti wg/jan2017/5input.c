#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
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
    float array[ARRAY_MAX]; // ARRAY_MAX = 1024
    unsigned arrayLen;
}OsInputData;

void *getMemBlock(char *path,int *size){
    int memFd = shm_open(path,O_RDWR,0); //000 su podrazumevana prava
    checkError(memFd!=-1,"shm_open");
    struct stat fInfo;
    checkError(fstat(memFd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;
    void *addr=mmap(NULL,*size, PROT_READ|PROT_WRITE, MAP_SHARED,memFd,0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(memFd);
    return addr;
}

int poredi(const void *br1, const void *br2){
    return (*(float*)br1 > *(float*)br2);
}

int main(int argc, char **argv){

    checkError(argc==2, "args");
    int size=0;
    OsInputData *memBlock = (OsInputData*)getMemBlock(argv[1],&size);
    checkError(sem_wait(&(memBlock->inDataReady))!=-1,"sem_wait");
    qsort(memBlock->array,memBlock->arrayLen,sizeof(float),poredi);
    printf("%f\n", memBlock->array[memBlock->arrayLen / 2]);
    checkError(munmap(memBlock,sizeof(memBlock))!=-1,"munmap");
    exit(EXIT_SUCCESS);
}
