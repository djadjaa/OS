#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <semaphore.h>
#include <sys/mman.h>

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define NAME_MAX 256

typedef struct{
    sem_t weekDayWritten;
    char weekDay[NAME_MAX];
}OsInputData;

void *getMemoryBlock(const char *filepath, unsigned *size){
    int fd = shm_open(filepath,O_RDWR,0);
    checkError(fd!=-1,"");
    struct stat fInfo;
    checkError(fstat(fd,&fInfo)!=-1,"");
    *size = fInfo.st_size;
    void *addr = mmap(NULL, *size, PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"");
    close(fd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    unsigned int size = 0;
    OsInputData *sharedObj = getMemoryBlock(argv[1],&size);
    checkError(sem_wait(&sharedObj->weekDayWritten)!=-1,"");

    printf("%s\n",sharedObj->weekDay);

    checkError(sem_post(&sharedObj->weekDayWritten)!=-1,"");
    checkError(munmap(sharedObj,sizeof(OsInputData))!=-1,"");
    

    exit(EXIT_SUCCESS);
}
