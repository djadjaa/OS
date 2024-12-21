#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <ctype.h>
#include <string.h>
#include <semaphore.h>

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
} OsInputData;

void *getMemBlock(const char *fPath,int *size){
    int memFd=shm_open(fPath,O_RDWR,0);
    checkError(memFd!=-1,"shm_open");
    struct stat fInfo;
    checkError(fstat(memFd,&fInfo)!=-1,"fstat");
    *size = fInfo.st_size;
    void *addr = mmap(NULL,*size,PROT_READ|PROT_WRITE,MAP_SHARED,memFd,0);
    checkError(addr!=MAP_FAILED,"mmap");
    close(memFd);
    return addr;
}
/*
char *inverse(char *s){
    int len = strlen(s);
    char *retStr = malloc(len+1);
    for(int i=0; i<len; i++){
        char c = s[i];
        if(isalpha(c)){
            if(islower(c)){
                retStr[i]=toupper(c);
            }else{
                retStr[i]=tolower(c);
            }
        }else{
            retStr[i]=c;
        }
    }
    retStr[len]=0;
    return retStr;
}
*/
int main(int argc, char **argv){
    checkError(argc==2,"args");
    int size=0;
    OsInputData *memData = (OsInputData*)getMemBlock(argv[1],&size);
    checkError(sem_wait(&(memData->inDataReady))!=-1,"sem wait");
    //char *retStr=inverse(memData->str);
    //umesto stampanja na standardni izlaz, rezultat postavljamo na semafor

    for(int i=0; memData->str[i]; i++){
        if(islower(memData->str[i])){
            memData->str[i] = toupper(memData->str[i]);
        }else if(isupper(memData->str[i])){
            memData->str[i] = tolower(memData->str[i]);
        }
    }

    checkError(sem_post(&(memData->dataProcessed))!=-1,"sem_post");
    //fprintf(stderr,"%s\n",memData->str);
    checkError(munmap(memData,sizeof(memData))!=-1,"munmap");
    //free(retStr);
}
