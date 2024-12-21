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
    float array[ARRAY_MAX];
    unsigned arrayLen;
}osData;

void *getMemoryBlock(const char *path, unsigned int *size){
    int fd = shm_open(path,O_RDWR,0);
    checkError(fd!=-1,"shmopen");

    struct stat fInfo;
    checkError(fstat(fd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;

    void *addr = mmap(NULL,*size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"mmap");

    close(fd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    unsigned int size=0;
    osData *sharedObj = getMemoryBlock(argv[1],&size);
    checkError(sem_wait(&sharedObj->inDataReady)!=-1,"sem wait");

    float sum = 0;
    for(int i=0; i<sharedObj->arrayLen; i++)
        sum += sharedObj->array[i];
    printf("%.2f\n",sum/sharedObj->arrayLen);
    checkError(sem_post(&sharedObj->inDataReady)!=-1,"sem post");
    checkError(munmap(sharedObj,size)!=-1,"Munmap");
    exit(EXIT_SUCCESS);
}