#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>

#define checkError(expr,usrMsg) \
    do{ \
        if (!(expr)){ \
            perror(usrMsg); \
            exit(1); \
        } \
    }while(0)

#define MAX_SIZE 1024

typedef struct {
    sem_t dataReady;
    sem_t operationDone;
    char array[MAX_SIZE];
    int resultSize;
    char result[MAX_SIZE];
}OsInputData;

void *createMemoryBlock(const char *pathname, unsigned size){

    int fd=shm_open(pathname,O_RDWR|O_CREAT,0600);
    checkError(fd!=-1,"shm_open");
    checkError(ftruncate(fd,size)!=-1,"ftruncate");
    void *addr=mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(fd);
    return addr;
}

int main(int argc, char **argv){
    checkError(argc==2,"args");

    OsInputData *sharedObj = createMemoryBlock(argv[1],sizeof(OsInputData));
    checkError(sem_init(&sharedObj->dataReady,1,0)!=-1,"sem_init");
    checkError(sem_init(&sharedObj->operationDone,1,0)!=-1,"SEM_INIT");

    scanf("%s",sharedObj->array);

    checkError(sem_post(&sharedObj->dataReady)!=-1,"sem_post");
    checkError(munmap(sharedObj,sizeof(OsInputData))!=-1,"munmap");
    
    exit(EXIT_SUCCESS);
}