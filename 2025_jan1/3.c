#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define pthreadCheck(pthreadErr, usrMsg) \
    do{ \
        int pthreadErrTmp = pthreadErr; \
        if(pthreadErrTmp > 0){ \
            errno = pthreadErrTmp; \
            checkError(false,usrMsg); \
        } \
    }while(0); \

int n,m;
int **mat;
int res = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
    int col;
}ThreadArg;

void *processThread(void *arg){
    ThreadArg *input = (ThreadArg*)arg;
    int localsum = 0;
    for(int i=0; i<n; i++){
        localsum+=mat[i][input->col];
        //printf(" local sum: %d",localsum);
    }
    pthreadCheck(pthread_mutex_lock(&mutex),"");

    if(localsum > res)
        res = localsum;

    pthreadCheck(pthread_mutex_unlock(&mutex),"");
    //printf("\n result:%d\n",res);
    return NULL;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    FILE *f = fopen(argv[1],"r");
    checkError(f!=NULL,"");
    fscanf(f,"%d%d",&n,&m);
    mat = malloc(n*sizeof(int*));
    checkError(mat!=NULL,"");
    for(int i=0; i<n; i++){
        mat[i] = malloc(m*sizeof(int));
        checkError(mat[i]!=NULL,"");
        for(int j=0; j<m; j++)
            fscanf(f,"%d", &mat[i][j]);
    }
    pthread_t *tids = malloc(m*sizeof(pthread_t));
    checkError(tids!=NULL,"");

    ThreadArg *data = malloc(m*sizeof(ThreadArg));
    checkError(data!=NULL,"");

    for(int j=0; j<m; j++){
        data[j].col = j;
        pthreadCheck(pthread_create(&tids[j],NULL,processThread,&data[j]),"");
    }

    for(int j=0; j<m; j++)
        pthreadCheck(pthread_join(tids[j],NULL),"");

    printf("%d\n",res);

    pthreadCheck(pthread_mutex_destroy(&mutex),"");
    free(data);
    free(tids);
    for(int i=0; i<n; i++)
        free(mat[i]);
    free(mat);
    fclose(f);
    exit(EXIT_SUCCESS);
}
