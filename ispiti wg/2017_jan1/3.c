#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
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
    }while(0);\

typedef struct{
    int row;
}ThreadArg;

int n,m;
double **mat;
double minres = INT_MAX*1.0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *processThread(void *arg) {
    ThreadArg *input = (ThreadArg*)arg;
    double min_row = INT_MAX*1.0;
    for(int j=0; j<m; j++){
        if(mat[input->row][j]<min_row)
            min_row = mat[input->row][j];
    }

    osPthreadCheck(pthread_mutex_lock(&mutex),"");

    if(min_row<minres)
        minres=min_row;

    osPthreadCheck(pthread_mutex_unlock(&mutex),"");

    return NULL;
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    FILE *f = fopen(argv[1],"r");
    checkError(f!=NULL,"fopen");
    fscanf(f,"%d%d",&n,&m);
    mat = (double**)malloc(n*sizeof(double*));
    checkError(mat!=NULL,"");
    for(int i=0; i<n; i++){
        mat[i] = (double*)malloc(m*sizeof(double));
        checkError(mat[i]!=NULL,"");
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            fscanf(f,"%lf",&mat[i][j]);
        }
    }
    pthread_t *tids = malloc(n*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    ThreadArg *data = malloc(n*sizeof(ThreadArg));
    checkError(data!=NULL,"");
    for(int i=0; i<n; i++){
        data[i].row = i;
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data[i]),"");
    }
    for(int i=0; i<n; i++)
        osPthreadCheck(pthread_join(tids[i],NULL),"");
    
    printf("%lf",minres);
    free(tids);
    free(data);
    for(int i=0; i<n; i++)
        free(mat[i]);
    free(mat);
    fclose(f);
    exit(EXIT_SUCCESS);
}