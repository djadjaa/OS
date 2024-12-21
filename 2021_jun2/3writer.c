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

//increment the semaphore value by one (sem_post(3)); and decrement the semaphore value by one (sem_wait(3)).
#define ARRAY_MAX 1024

typedef struct{
    sem_t inDataReady;
    int array[ARRAY_MAX];
    unsigned arrayLen;
}osData;

void *createMemoryBlock(const char *path, unsigned size){
    int fd = shm_open(path,O_RDWR|O_CREAT,0600);
    checkError(fd!=-1,"fd");
    checkError(ftruncate(fd,size)!=-1,"ftruncate");
    void *addr = mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(fd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    osData *input = createMemoryBlock(argv[1],sizeof(osData));
    osData *output = createMemoryBlock(argv[2],sizeof(osData));
    checkError(sem_init(&input->inDataReady,1,1)!=-1,"init input");
    checkError(sem_init(&output->inDataReady,1,0)!=-1,"init output");
    scanf("%u",&input->arrayLen);
    for(int i=0; i<input->arrayLen; i++)
        scanf("%d", &input->array[i]);
    checkError(sem_post(&input->inDataReady)!=-1,"sem post"); //reader deo
    checkError(sem_wait(&output->inDataReady)!=-1,"sem wait");
    exit(EXIT_SUCCESS);
}