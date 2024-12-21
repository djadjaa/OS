#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <ctype.h>

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \


#define checkPthread(err,userMsg)\
    do{\
        if(err > 0){\
            errno=err;\
            checkError(0,userMsg);\
        }\
    }while(0); \

typedef struct{
    int numCh;
    char letter;
}InputData;

const int n=26;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
char maxSlovo;
int maxNum=0;
char *filePath=NULL;

void *threadFunc(void *a){
    InputData *arg = (InputData*)a;

    FILE *f = fopen(filePath,"r");
    checkError(f!=NULL,"fopen");

    InputData *tmp = malloc(sizeof(InputData));
    checkError(tmp!=NULL,"malloc");
    tmp->letter = arg->letter;
    tmp->numCh = arg->numCh;
    char c;
    while((c=fgetc(f))!=EOF){
        if(tolower(c) == arg->letter)
            tmp->numCh++;
    }
    fclose(f);

    checkPthread(pthread_mutex_lock(&mut)!=-1,"lock");

    if(tmp->numCh > maxNum){
        maxSlovo = tmp->letter;
        maxNum = tmp->numCh;
    }

    checkPthread(pthread_mutex_unlock(&mut)!=-1,"unlock");

    return tmp;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    filePath = argv[1];

    pthread_t *tids=malloc(n*sizeof(pthread_t));
    checkError(tids!=NULL,"malloc");
    InputData *data = malloc(n*sizeof(InputData));
    checkError(data!=NULL,"malloc");

    for(int i=0; i<n; i++){
        data[i].letter = 'a'+i;
        data[i].numCh = 0;
        checkPthread(pthread_create(&tids[i],NULL,threadFunc,&data[i])!=-1,"pthread create");
    }

    for(int i=0; i<n; i++){
        InputData *res=NULL;
        checkPthread(pthread_join(tids[i],(void**)&res),"pthread join");
        printf("%d ", res->numCh);
        free(res);
    }
    checkPthread(pthread_mutex_destroy(&mut)!=-1,"destroy");
    printf("\n");
    printf("%c\n",maxSlovo);
    free(data);
    free(tids);
    exit(EXIT_SUCCESS);
}
