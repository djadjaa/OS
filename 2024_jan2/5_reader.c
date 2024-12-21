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

#define MAX_ARRAY 1024
typedef struct{
    sem_t inDataReady;
    sem_t averageCalculated;
    float array[MAX_ARRAY];
    unsigned arrayLen;
    float average;
}shm_obj;

void *getMemoryBlock(const char *filepath, unsigned *size) {
    int fd= shm_open(filepath,O_RDWR,0);
    checkError(fd!=-1,"shm_open");
    
    struct stat fInfo;
    checkError(fstat(fd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;
    void *addr = mmap(NULL,*size,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(fd);
    return addr;
}

float aritmetickaSredina(float *array, unsigned arrayLen){
    float sum=0;
    for(int i=0; i<arrayLen; i++){
        sum += array[i];
    }
    sum /= arrayLen;
    return sum;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    unsigned int size = 0;
    shm_obj *sharedObj = getMemoryBlock(argv[1],&size);
    
    checkError(sem_wait(&sharedObj->inDataReady)!=-1,"sem_wait");

    sharedObj->average = aritmetickaSredina(sharedObj->array, sharedObj->arrayLen);

    checkError(sem_post(&sharedObj->averageCalculated)!=-1,"sem_post");
    checkError(munmap(sharedObj,sizeof(shm_obj))!=-1,"munmap");
    
    exit(EXIT_SUCCESS);
}
