#define _XOPEN_SOURCE  500
#define _POSIX_C_SOURCE 200112L
#define ARRAY_MAX 1024
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <semaphore.h>

#define checkError(expr,usrMsg) \
    do { \
        if(!(expr)){ \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0); \

typedef struct {
    sem_t dataProcessingFinished;
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

int usr1caught=0,usr2caught=0;

void signalHandler(int signum){
    switch(signum){
        case(SIGUSR1):
            usr1caught=1;
            break;
        case(SIGUSR2):
            usr2caught=1;
            break;
    }
}

void* getMemBlock(const char *fName, int size){
    int memFd = shm_open(fName,O_RDWR | O_CREAT | O_TRUNC ,0644);
    checkError(memFd!=-1,"shm_open");
    checkError(ftruncate(memFd,size)!=-1,"ftruncate");
    void *addr=mmap(NULL,size,PROT_READ | PROT_WRITE, MAP_SHARED,memFd,0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(memFd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    OsInputData *data = getMemBlock(argv[1],sizeof(OsInputData));

    scanf("%d",&data->arrayLen);
    for(int i=0; i<data->arrayLen; i++)
        scanf("%d",&data->array[i]);

    checkError(munmap(data,sizeof(OsInputData))!=-1,"munmap");
    exit(EXIT_SUCCESS);
}
