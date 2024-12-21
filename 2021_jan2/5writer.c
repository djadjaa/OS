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

#define ARRAY_MAX 1024

typedef struct {
    sem_t inDataReady;
    sem_t dataProcessed;
    char str[ARRAY_MAX];
}shm_obj;

void *createMemoryBlock(const char *path, unsigned int size){

    int fd = shm_open(path,O_RDWR|O_CREAT,0600);
    checkError(fd!=-1,"shm_open");
    checkError(ftruncate(fd,size)!=-1,"ftruncate");
    void *addr=mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=NULL,"mmap");
    close(fd);
    
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    shm_obj *sharedObj = createMemoryBlock(argv[1],sizeof(shm_obj));
    checkError(sem_init(&sharedObj->inDataReady,1,1)!=-1,"sem init");
    checkError(sem_init(&sharedObj->dataProcessed,1,0)!=-1,"SEM init");
    scanf("%s",sharedObj->str);
    checkError(sem_post(&sharedObj->inDataReady)!=-1,"sem post"); //reader deo
    checkError(munmap(sharedObj,sizeof(shm_obj))!=-1,"munmap");

    exit(EXIT_SUCCESS);
}