#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define NAME_MAX 256

typedef struct {
    sem_t inDataReady;
    sem_t operationDone;
    char weekDay[NAME_MAX];
    int timeInSeconds;
}OsInputData;

void *getMemoryBlock(const char *path, unsigned int *size){
    int fd = shm_open(path, O_RDWR, 0);
    checkError(fd!=-1,"shm open");
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
    unsigned int size = 0;
    OsInputData *sharedObj = getMemoryBlock(argv[1],&size);
    checkError(sem_wait(&sharedObj->inDataReady)!=-1,"sem wait");

    time_t now = 0;
    checkError(time(&now)!=-1,"time");
    time_t numsec = sharedObj->timeInSeconds;
    struct tm *time = localtime(&numsec);
    checkError(time!=NULL,"local time");
    switch(time->tm_wday){
        case 0:
            strcpy(sharedObj->weekDay,"nedelja");
            break;
        case 1:
            strcpy(sharedObj->weekDay,"ponedeljak");
            break;
        case 2:
            strcpy(sharedObj->weekDay,"utorak");
            break;
        case 3:
            strcpy(sharedObj->weekDay,"sreda");
            break;
        case 4:
            strcpy(sharedObj->weekDay,"cetvrtak");
            break;
        case 5:
            strcpy(sharedObj->weekDay,"petak");
            break;
        case 6:
            strcpy(sharedObj->weekDay,"subota");
            break;
    }
    printf("%s\n",sharedObj->weekDay);
    checkError(sem_post(&sharedObj->operationDone)!=-1,"sem post");
    checkError(munmap(sharedObj,size)!=-1,"Munmap");
    exit(EXIT_SUCCESS);
}