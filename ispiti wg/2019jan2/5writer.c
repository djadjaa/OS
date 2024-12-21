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

void *getMemBlock(const char *fName, int size){
    //int shm_open(const char *name, int oflag, mode_t mode);
    int memFd = shm_open(fName, O_RDWR | O_CREAT | O_TRUNC, 0600);
    checkError(memFd!=-1,"shm_open");
    checkError(ftruncate(memFd,size)!=-1,"ftrunate");
    void *addr = mmap(NULL,size,PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(memFd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    OsData *input = getMemBlock(argv[1],sizeof(OsData));
    OsData *output = getMemBlock(argv[2],sizeof(OsData));
    //0 - deljena memorija između niti
    checkError(sem_init(&(input->inDataReady),0,1)!=-1,"sem_init"); //slobodan
    checkError(sem_init(&(output->inDataReady),0,0)!=-1,"sem_init"); //zauzet
    unsigned n;
    scanf("%u",&n);
    input->arrayLen = n;
    output->arrayLen = 0;
    for(int i=0; i<n; i++){
        scanf("%d",&input->array[i]);
    }
    checkError(munmap(input,sizeof(OsData))!=-1,"munmap");
    checkError(munmap(output,sizeof(OsData))!=-1,"munmap");

    exit(EXIT_SUCCESS);
}
