#define _POSIX_C_SOURCE 200112L
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>

#define checkError(expr,usrMsg) \
    do { \
        if(!(expr)){ \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0); \

#define checkPthread(err,usrMsg) \
    do { \
        if(err>0){\
            errno=err;\
            checkError(0,usrMsg);\
        }\
    }while(0);\

typedef struct{
    int vrsta;
}InputArgs_t;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER; //MUTEKS
double sum=0;
int m,n;
//m vrste n kolone
double p;
double** a = NULL;

void *threadFunc(void *b){
    InputArgs_t* data = (InputArgs_t*)b;
    double tempsum=0;
    for(int i=0; i<n; i++){
        tempsum += pow(fabs(a[data->vrsta][i]),p);
    }

    checkPthread(pthread_mutex_lock(&mut),"lock");
    sum += tempsum;
    checkPthread(pthread_mutex_unlock(&mut),"unlock");
    return NULL;
}

int main(int argc, char **argv){

    scanf("%lf%d%d",&p,&m,&n);
    a = malloc(m*sizeof(double*));
    checkError(a!=NULL,"malloc");
    for(int i=0; i<m; i++){
        a[i] = malloc(n*sizeof(double));
    }

    for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){
            scanf("%lf",&a[i][j]);
        }
    }

    pthread_t *tids = malloc(m*sizeof(pthread_t));
    checkError(tids!=NULL,"malloc");
    InputArgs_t *data = malloc(m*sizeof(InputArgs_t));
    checkError(data!=NULL,"malloc");


    for(int i=0; i<m; i++){
        data[i].vrsta = i;

        checkPthread(pthread_create(&tids[i],NULL,threadFunc,&data[i]),"pthread_create");
    }

    for(int i=0; i<m; i++){
        checkPthread(pthread_join(tids[i],NULL),"pthread_join");
    }

    double norma = pow(sum,1.0/p);
    printf("%.4lf\n",norma);

    free(data);
    free(tids);
    for(int i=0; i<m; i++){
        free(a[i]);
    }
    free(a);
    exit(EXIT_SUCCESS);
}
