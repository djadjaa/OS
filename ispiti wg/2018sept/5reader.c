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

void* getMemBlock(const char *fName, int *size){
    int memFd = shm_open(fName,O_RDWR,0600);
    checkError(memFd!=-1,"shm_open");
    struct stat fInfo;
    checkError(fstat(memFd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;
    void *addr=mmap(NULL,*size,PROT_READ | PROT_WRITE, MAP_SHARED,memFd,0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(memFd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int size=0;
    OsInputData *data = getMemBlock(argv[1],&size);
    checkError(signal(SIGUSR1,signalHandler)!=SIG_ERR,"signal");
    checkError(signal(SIGUSR2,signalHandler)!=SIG_ERR,"signal");
    pause();
    if(usr1caught){
        for(int i=0; i<data->arrayLen; i++){
            data->array[i] *= -1;
        }
        usr1caught=0;
    }else if(usr2caught){
        for(int i=0; i<data->arrayLen; i++){
            data->array[i] *= 2;
        }
        usr2caught=0;
    }
    sem_post(&data->dataProcessingFinished);
    exit(EXIT_SUCCESS);
}
