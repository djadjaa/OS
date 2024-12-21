#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>
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

void *getMemoryBlock(const char *pathname, unsigned *size){

    int fd=shm_open(pathname,O_RDWR,0);
    checkError(fd!=-1,"shm_open");
    
    struct stat fileInfo;
    checkError(fstat(fd,&fileInfo)!=-1,"stat");
    *size = fileInfo.st_size;

    void *addr=mmap(NULL,*size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(fd);
    return addr;
}

int main(int argc, char **argv){
    checkError(argc==2,"args");

    unsigned int size=0;

    OsInputData *sharedObj = getMemoryBlock(argv[1],&size);
    checkError(sem_wait(&sharedObj->dataReady)!=-1,"sem_init");
    
    int j=0;
    for(int i=0; i<strlen(sharedObj->array); i++){
        if(isdigit(sharedObj->array[i])){
            sharedObj->result[j] = sharedObj->array[i];
            j++;
        }
    }
    sharedObj->resultSize = j;
    printf("%d\n",sharedObj->resultSize);

    checkError(sem_post(&sharedObj->operationDone)!=-1,"sem_post");
    checkError(munmap(sharedObj,sizeof(OsInputData))!=-1,"munmap");
    
    exit(EXIT_SUCCESS);
}