#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <string.h>
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
}OsInputData;

void *getMemoryBlock(const char *path, unsigned *size){
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

int main(int argc, char **argv){

    checkError(argc==2,"args");
    unsigned size=0;
    OsInputData *sharedObj = getMemoryBlock(argv[1],&size);
    checkError(sem_wait(&sharedObj->inDataReady)!=-1,"sem wait");
    
    int i=0, j=strlen(sharedObj->str)-1;
    char tmp;

    while(i<j){
        tmp = sharedObj->str[i];
        sharedObj->str[i] = sharedObj->str[j];
        sharedObj->str[j] = tmp;
        i++;
        j--;
    }

    printf("%s\n",sharedObj->str);

    checkError(sem_post(&sharedObj->dataProcessed)!=-1,"");
    checkError(munmap(sharedObj,size)!=-1,"");
    exit(EXIT_SUCCESS);
}