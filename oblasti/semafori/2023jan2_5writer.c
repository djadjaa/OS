#define _XOPEN_SOURCE 500
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

typedef struct{
    sem_t dataReady;
    sem_t operationDone;
    int array[MAX];
    int arraySize;
    int result[MAX];
}OsInputData;

void *createMemoryBlock(const char *fpath, unsigned int size){
    int fd = shm_open(fpath,O_RDWR|O_CREAT,0600);
    checkError(fd!=-1,"");

    checkError(ftruncate(fd,size)!=-1,"");;

    void *addr = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"");
    close(fd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    OsInputData *sharedObj = createMemoryBlock(argv[1],sizeof(OsInputData));

    checkError(sem_init(&sharedObj->dataReady,1,1)!=-1,"");
    checkError(sem_init(&sharedObj->operationDone,1,0)!=-1,"");

    scanf("%d",&sharedObj->arraySize);

    for(int i=0; i<sharedObj->arraySize; i++){
        scanf("%d",&sharedObj->array[i]);
    }

    checkError(sem_post(&sharedObj->dataReady)!=-1,"");
    checkError(munmap(sharedObj,sizeof(OsInputData))!=-1,"");

    exit(EXIT_SUCCESS);
}
