#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define osPthreadCheck(err,usrMsg)\
    do{\
        int pthreadErr = err;\
        if(pthreadErr>0){\
            errno = pthreadErr;\
            checkError(false,"usrMsg");\
        }\
    }while(0);

int n,m;
int **mat;

int globalmax=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
    int column;
}ThreadArgs;

typedef struct{
    int sumofcolumn;
}Output;


void *processThread(void *args){
    ThreadArgs *input = (ThreadArgs*)args;
    int localsum=0;
    for(int i=0; i<n; i++){
        localsum += mat[i][input->column];
    }
    osPthreadCheck(pthread_mutex_lock(&mutex),"");
    if(localsum>globalmax)
        globalmax=localsum;
    osPthreadCheck(pthread_mutex_unlock(&mutex),"");

    Output *output = malloc(sizeof(Output));
    checkError(output!=NULL,"");
    output->sumofcolumn = localsum;
    return output;
}

int main(int argc, char **argv){

    scanf("%d%d",&n,&m);
    mat = malloc(n*sizeof(int*));
    checkError(mat!=NULL,"");
    for(int i=0; i<n; i++){
        mat[i] = malloc(m*sizeof(int));
        checkError(mat[i]!=NULL,"");
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            scanf("%d",&mat[i][j]);
        }
    }
    pthread_t *tids = malloc(m*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    ThreadArgs *data = malloc(m*sizeof(ThreadArgs));
    checkError(data!=NULL,"");
    for(int j=0; j<m; j++){
        data[j].column = j;
        osPthreadCheck(pthread_create(&tids[j],NULL,processThread,&data[j]),"");
    }
    for(int j=0; j<m; j++){
        Output *data_output = NULL;
        osPthreadCheck(pthread_join(tids[j],(void**)&data_output),"");
        printf("%d ",data_output->sumofcolumn);
        free(data_output);
    }
    printf("\n%d\n",globalmax);
    free(tids);
    free(data);
    for(int i=0; i<n; i++)
        free(mat[i]);
    free(mat);
    exit(EXIT_SUCCESS);
}
