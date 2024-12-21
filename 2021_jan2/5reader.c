#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <ctype.h>
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
}shm_obj;

void *getMemoryBlock(const char *path, unsigned *size){
    int fd = shm_open(path,O_RDWR,0);
    checkError(fd!=-1,"shm open");

    struct stat fInfo;
    checkError(fstat(fd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;

    void *addr = mmap(NULL,*size, PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(fd);
    return addr;
}

void digitToHashtag(char *s){
    for(int i=0; s[i]!='\0'; i++){
        if(isdigit(s[i]))
            s[i] = '#';
    }
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    unsigned size = 0;
    shm_obj *sharedObj = getMemoryBlock(argv[1],&size);
    checkError(sem_wait(&sharedObj->inDataReady)!=-1,"sem wait");
    digitToHashtag(sharedObj->str);
    printf("%s\n",sharedObj->str);
    checkError(sem_post(&sharedObj->dataProcessed)!=-1,"sem post");
    checkError(munmap(sharedObj,sizeof(shm_obj))!=-1,"Munmap");
    exit(EXIT_SUCCESS);
}