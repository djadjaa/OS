#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <stdbool.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define osPthreadCheck(err,usrMsg) \
    do{\
        int pthreadErr = err;\
        if(pthreadErr>0){\
            errno = pthreadErr;\
            checkError(0,"usrMsg");\
        }\
    }while(0);\

int n,m;
int **mat;
int globalMaxsum=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool initialized = false;

typedef struct {
    int col;
}threadArg;

void *processThread(void *arg){
    threadArg *input = (threadArg*)arg;
    int localsum=0;
    for(int i=0; i<n; i++){
        localsum+=mat[i][input->col];
    }
    osPthreadCheck(pthread_mutex_lock(&mutex),"mutex lock");

    if(localsum>globalMaxsum)
        globalMaxsum=localsum;

    osPthreadCheck(pthread_mutex_unlock(&mutex),"mutex unlock");

    return NULL;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    FILE *f = fopen(argv[1],"r");
    checkError(f!=NULL,"fopen");
    fscanf(f,"%d %d",&n,&m);
    mat = malloc(n*sizeof(int*));
    checkError(mat!=NULL,"malloc");
    for(int i=0; i<n; i++){
        mat[i]=malloc(m*sizeof(int));
        checkError(mat[i]!=NULL,"malloc");
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            fscanf(f,"%d",&mat[i][j]);
        }
    }
    pthread_t *tids = malloc(m*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    threadArg *data = malloc(m*sizeof(threadArg));
    checkError(data!=NULL,"");

    for(int j=0; j<m; j++){
        data[j].col = j;
        osPthreadCheck(pthread_create(&tids[j],NULL,processThread,&data[j]),"pthread create");
    }
    for(int j=0; j<m; j++){
        osPthreadCheck(pthread_join(tids[j],NULL),"pthread join");
    }
    osPthreadCheck(pthread_mutex_destroy(&mutex),"destroy");
    printf("%d\n",globalMaxsum);
    free(tids);
    free(data);
    for(int i=0; i<n; i++)
        free(mat[i]);
    free(mat);
    exit(EXIT_SUCCESS);
}