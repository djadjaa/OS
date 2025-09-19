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
    sem_t inDataReady;
    sem_t averageCalculated;
    float array[MAX];
    unsigned arrayLen;
    float average;
}OsInputData;

void *getMemoryBlock (const char *path, unsigned *size){
    int fd = shm_open(path,O_RDWR,0);
    checkError(fd!=-1,"");

    struct stat fInfo;
    checkError(fstat(fd,&fInfo)!=-1,"");
    *size = fInfo.st_size;

    void *addr = mmap(NULL,*size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"");
    close(fd);
    return addr;
}

float aritmetickasredina(float *array, unsigned n){
    float sum = 0;
    for(int i=0; i<n; i++)
        sum += array[i];
    sum /= n;
    return sum;
}

int main(int argc, char **argv){
    checkError(argc==2,"");
    unsigned int size=0;
    OsInputData *sharedObj = getMemoryBlock(argv[1],&size);

    checkError(sem_wait(&sharedObj->inDataReady)!=-1,"");

    sharedObj->average = aritmetickasredina(sharedObj->array,sharedObj->arrayLen);

    printf("%f\n",sharedObj->average);
    checkError(sem_post(&sharedObj->averageCalculated)!=-1,"");
    checkError(munmap(sharedObj,size)!=-1,"");


    exit(EXIT_SUCCESS);
}
