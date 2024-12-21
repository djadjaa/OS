#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>

#include <semaphore.h>
#include <sys/mman.h>
#define MAX_ARRAY 1024
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

typedef struct{
    sem_t inDataReady;
    sem_t averageCalculated;
    float array[MAX_ARRAY];
    unsigned arrayLen;
    float average;
}shm_obj;

void *createMemoryBlock(const char *filepath, unsigned size) {
    int fd= shm_open(filepath,O_RDWR|O_CREAT,0600);
    checkError(fd!=-1,"shm_open");
    checkError(ftruncate(fd,size)!=-1,"ftruncate");
    void *addr = mmap(0,size,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(fd);
    return addr;
}


int main(int argc, char **argv){

    checkError(argc==2,"args");
    shm_obj *sharedObj = createMemoryBlock(argv[1],sizeof(shm_obj));
    checkError(sem_init(&sharedObj->inDataReady,1,1)!=-1,"sem_init");
    checkError(sem_init(&sharedObj->averageCalculated,1,0)!=-1,"sem_init");

    float number;
    sharedObj->arrayLen = 0; //trenutno imamo 0 elem
    while(scanf("%f",&number)!=EOF){
        sharedObj->array[sharedObj->arrayLen] = number;
        sharedObj->arrayLen++;
    }
    checkError(sem_post(&sharedObj->inDataReady)!=-1,"sem_post");
    checkError(munmap(sharedObj,sizeof(shm_obj))!=-1,"munmap");
    
    exit(EXIT_SUCCESS);
}
