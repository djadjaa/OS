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
    int memFd =shm_open(fName, O_RDWR, 0000);
    checkError(memFd!=-1,"shm_open");
    struct stat fInfo;
    checkError(fstat(memFd,&fInfo)!=-1,"fstat")
    *size = fInfo.st_size;
    void *addr = mmap(NULL,*size,PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(memFd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int size = 0;
    //ovde ćemo razmatrati samo izlaznu memoriju
    OsData *output = getMemBlock(argv[2],&size);
    for(int i=0; i<output->arrayLen; i++){
        printf("%d ", output->array[i]);
    }
    checkError(munmap(output,size)!=-1,"munmap");
    exit(EXIT_SUCCESS);
}
