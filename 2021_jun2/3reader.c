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
    int array[ARRAY_MAX];
    unsigned arrayLen;
}osData;

//increment the semaphore value by one (sem_post(3)); and decrement the semaphore value by one (sem_wait(3)).

void *getMemoryBlock(const char *path, unsigned *size){
    int fd = shm_open(path, O_RDWR, 0);
    checkError(fd!=-1,"shm open");

    struct stat fInfo;
    checkError(fstat(fd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;

    void *addr = mmap(NULL,*size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"Mmap");

    close(fd);
    return addr;
}

int checkBits(int x){
    int mask = 1;
    int numBits=0;
    while(mask){
        if(x&mask)
            numBits++;
        mask <<= 1;
    }
    return numBits >= 4;
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    unsigned sizeInput=0, sizeOutput=0;
    osData *input = getMemoryBlock(argv[1],&sizeInput);
    osData *output = getMemoryBlock(argv[2],&sizeOutput);
    checkError(sem_wait(&input->inDataReady)!=-1,"sem wait");

    output->arrayLen=0;

    for(int i=0; i<input->arrayLen; i++){
        if(checkBits(input->array[i])){
            output->array[output->arrayLen] = input->array[i];
            output->arrayLen++;
        }
    }

    printf("%u\n",output->arrayLen);
    for(int i=0; i<output->arrayLen; i++){
        printf("%d ", output->array[i]);
    }
    putchar('\n');

    checkError(sem_post(&output->inDataReady)!=-1,"sem post");

    checkError(munmap(input,sizeInput)!=-1,"input munmap");
    checkError(munmap(output,sizeOutput)!=-1,"output munmap");

    exit(EXIT_SUCCESS);
}