//semwait će zaključati semafor i obaviti sve što treba dok semafor čeka i sempost će otključati semafor i omogućiti dalji tok programa
//ovaj between program će imati istu semafor fju kao što ima čitač, a u mejnu će imati fje sem wait i sempost gde će se između njih obaviti ono što se zahteva
#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>

#define checkError(expr,usrMsg) \
    do { \
        if(!(expr)){ \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0); \

#define ARRAY_MAX 1024

typedef struct {
    sem_t inDataReady;
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsData;

void *getMemBlock(const char *fName, int *size){
    int memFd = shm_open(fName,O_RDWR,0000);
    checkError(memFd!=-1,"shm_open");

    struct stat fInfo;
    checkError(fstat(memFd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;

    void *addr = mmap(NULL,*size,PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
    checkError(addr!=MAP_FAILED,"mmap");
    return addr;
}

int numUnits(int x){
    int num = 0;
    while(x){
        if(x&1)
            num++;
        x >>= 1;
    }
    return num;
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    int inputSize=0, outputSize=0;
    OsData *input = getMemBlock(argv[1],&inputSize);
    OsData *output = getMemBlock(argv[2],&outputSize);
    checkError(sem_wait(&(input->inDataReady))!=-1,"sem_wait");

    int num=0;
    for(int i=0; i<input->arrayLen; i++){
        if(numUnits(input->array[i])>=4){
            output->array[num] = input->array[i];
            num++;
        }
    }
    output->arrayLen = num;

    checkError(sem_post(&(output->inDataReady))!=-1,"sem_post");

    checkError(munmap(input,inputSize)!=-1,"munmap");
    checkError(munmap(output,outputSize)!=-1,"munmap");
    exit(EXIT_SUCCESS);
}
