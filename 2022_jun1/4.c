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
#include <math.h>
#include <libgen.h>
#include <string.h>
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
            errno=pthreadErr;\
            checkError(false,"usrMsg");\
        }\
    }while(0);

int n,m;
int **mat;

typedef struct{
    int idx;
}ThreadArgs;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int maxnum=INT_MIN;

void *processThread(void *args){
    ThreadArgs *input = (ThreadArgs*)args;
    int localmax= INT_MIN;
    for(int j=0; j<m; j++){
        if(mat[input->idx][j]>localmax){
            localmax = mat[input->idx][j];
        }
    }
    osPthreadCheck(pthread_mutex_lock(&mutex),"");

    if(localmax>maxnum)
        maxnum=localmax;
    
    osPthreadCheck(pthread_mutex_unlock(&mutex),"");
    return NULL;
}

int main(int argc, char **argv){
    checkError(argc==2,"args");
    FILE *f = fopen(argv[1],"r");
    checkError(f!=NULL,"fopen");

    fscanf(f,"%d",&n);

    mat=(int**)malloc(n*sizeof(int*));
    checkError(mat!=NULL,"");

    for(int i=0; i<n; i++){
        fscanf(f,"%d",&m);
        mat[i]=(int*)malloc(m*sizeof(int));
        checkError(mat[i]!=NULL,"");
        for(int j=0; j<m; j++)
            fscanf(f,"%d",&mat[i][j]);
    }
    pthread_t *tids = malloc(n*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    ThreadArgs *data = malloc(n*sizeof(ThreadArgs));
    checkError(data!=NULL,"");

    for(int i=0; i<n; i++){
        data[i].idx = i;
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data[i]),"");
    }
    for(int i=0; i<n; i++){
        osPthreadCheck(pthread_join(tids[i],NULL),"");
    }
    printf("%d\n",maxnum);
    free(tids);
    free(data);
    for(int i=0; i<n; i++)
        free(mat[i]);
    free(mat);
    fclose(f);
    exit(EXIT_SUCCESS);
}