#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 1024

typedef struct {
    sem_t inDataReady;
    sem_t dataProcessed;
    char str[MAX];
}OsInputData;

void *createMemoryBlock(const char *path, unsigned size){
    int fd = shm_open(path,O_RDWR|O_CREAT,0600);
    checkError(fd!=-1,"");
    checkError(ftruncate(fd,size)!=-1,"");
    void *addr = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"");
    close(fd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    OsInputData *sharedObj = createMemoryBlock(argv[1],sizeof(OsInputData));
    checkError(sem_init(&sharedObj->inDataReady,1,1)!=-1,"");
    checkError(sem_init(&sharedObj->dataProcessed,1,0)!=-1,"");
    scanf("%s",sharedObj->str);
    checkError(sem_post(&sharedObj->inDataReady)!=-1,""); //reader
    checkError(munmap(sharedObj,sizeof(sharedObj))!=-1,"");
    exit(EXIT_SUCCESS);
}
