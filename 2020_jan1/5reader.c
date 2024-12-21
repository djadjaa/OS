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

void *getMemoryBlock(const char *path, unsigned int *size){

    int fd = shm_open(path,O_RDWR,0);
    checkError(fd!=-1,"fd");

    struct stat fInfo;
    checkError(fstat(fd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;

    void *addr = mmap(NULL,*size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(fd);
    return addr;
}

void malaUVelika(char *s){
    for(int i=0; s[i]!='\0'; i++){
        if(isupper(s[i]))
            s[i] = tolower(s[i]);
        else if(islower(s[i]))
            s[i] = toupper(s[i]);
    }
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    unsigned int size = 0;
    shm_obj *sharedObj = getMemoryBlock(argv[1],&size);
    checkError(sem_wait(&sharedObj->inDataReady)!=-1,"sem wait");
    malaUVelika(sharedObj->str);
    printf("%s\n",sharedObj->str);
    checkError(sem_post(&sharedObj->dataProcessed)!=-1,"sem_post");
    checkError(munmap(sharedObj,sizeof(shm_obj))!=-1,"munmap");
    exit(EXIT_SUCCESS);
}