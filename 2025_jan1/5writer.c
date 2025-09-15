#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
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

void *createMemoryBlock (const char *filepath, unsigned size){
    int fd = shm_open(filepath, O_RDWR|O_CREAT,0600);
    checkError(fd!=-1,"");
    checkError(ftruncate(fd,size)!=-1,"");
    void *addr = mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"");
    close(fd);
    return addr;
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    OsInputData *sharedObj = createMemoryBlock(argv[1], sizeof(OsInputData));
    checkError(sem_init(&sharedObj->weekDayWritten,1,0)!=-1,"");
    
    time_t now = time(NULL);
    struct tm *brokenTime = localtime(&now);
    checkError(brokenTime!=NULL,"");
    if(brokenTime->tm_wday==0)
        strcpy(sharedObj->weekDay,"nedelja");
    else if(brokenTime->tm_wday==1)
        strcpy(sharedObj->weekDay,"ponedeljak");
    else if(brokenTime->tm_wday==2)
        strcpy(sharedObj->weekDay,"utorak");
    else if(brokenTime->tm_wday==3)
        strcpy(sharedObj->weekDay,"sreda");
    else if(brokenTime->tm_wday==4)
        strcpy(sharedObj->weekDay,"cetvrtak");
    else if(brokenTime->tm_wday==5)
        strcpy(sharedObj->weekDay,"petak");
    else if(brokenTime->tm_wday==6)
        strcpy(sharedObj->weekDay,"subota");

    checkError(sem_post(&sharedObj->weekDayWritten)!=-1,"");
    checkError(munmap(sharedObj,sizeof(OsInputData))!=-1,"");

    exit(EXIT_SUCCESS);
}
