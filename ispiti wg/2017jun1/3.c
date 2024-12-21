#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
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
    }while(0) \

typedef struct{
    int red,kolona;
}InputArgs_t;

int **A;
int **B;
int **C;
int n,m,k,maxRes=INT_MIN;
pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER; //inicijalizacija našeg muteksa

void *threadFunc(void *a){

    InputArgs_t* data=(InputArgs_t*)a;
    int localCounter=0;

    //mnozenje matrice
    for(int i=0; i<m; i++)
        localCounter += A[data->red][i]*B[i][data->kolona];

    checkPthread(pthread_mutex_lock(&g_lock), "lock");
    //muteksi sada citaju zajednicke resurse
    C[data->red][data->kolona] = localCounter;
    if(localCounter>maxRes)
        maxRes=localCounter;
    checkPthread(pthread_mutex_unlock(&g_lock), "unlock");
    return NULL;
}

int main(int argc, char **argv){

    scanf("%d%d",&n,&m);
    A = malloc(n*sizeof(int*));
    checkError(A!=NULL,"malloc");

    for(int i=0; i<n; i++){
        A[i] = malloc(m*sizeof(int));
        checkError(A[i]!=NULL,"malloc");
    }

    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            scanf("%d",&A[i][j]);
        }
    }

    scanf("%d%d",&m,&k);

    B = malloc(m*sizeof(int*));
    checkError(B!=NULL,"malloc");
    for(int i=0; i<m; i++){
        B[i] = malloc(k*sizeof(int));
        checkError(B[i]!=NULL,"malloc");
    }

    for(int i=0; i<m; i++){
        for(int j=0; j<k; j++){
            scanf("%d",&B[i][j]);
        }
    }

    C = malloc(n*sizeof(int*));
    checkError(C!=NULL,"malloc");
    for(int i=0; i<n; i++){
        C[i] = malloc(k*sizeof(int));
        checkError(C[i]!=NULL,"malloc");
    }

    //PID Stands for Process ID and TID stands for Thread ID
    pthread_t *pids = malloc(n*k*sizeof(pthread_t));
    checkError(pids!=NULL,"malloc");
    InputArgs_t *data = malloc(n*k*sizeof(InputArgs_t));
    checkError(data!=NULL,"malloc");

    for(int i=0; i<n; i++){
        for(int j=0; j<k; j++){
            data[i*k+j].red = i;
            data[i*k+j].kolona=j;
            //int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);

            checkPthread(pthread_create(&pids[i*k+j],NULL,threadFunc,&data[i*k+j]),"pthread_create");
        }
    }

    for(int i=0; i<n*k; i++)
        checkPthread(pthread_join(pids[i],NULL),"pthread_join");
    checkPthread(pthread_mutex_destroy(&g_lock),"pthread_mutex_destroy");

    for(int i=0; i<n; i++){
        for(int j=0; j<k; j++){
            printf("%d ",C[i][j]);
        }
        printf("\n");
    }
    printf("%d\n",maxRes);

    free(data);
    free(pids);

    for(int i=0; i<n; i++)
        free(C[i]);

    free(C);

    for(int i=0; i<m; i++)
        free(B[i]);

    free(B);

    for(int i=0; i<n; i++)
        free(A[i]);

    free(A);

    return 0;
}
