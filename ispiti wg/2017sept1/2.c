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
    }while(0); \

typedef struct{
    int indeks;
}InputArgs_t;

int **A;
pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER; //inicijalizacija našeg muteksa
int m;
int maxres=INT_MIN,resind=0;

void *threadFunc(void *a){

    InputArgs_t* data=(InputArgs_t*)a;

    int localres=0,i;
    for(i=0; i<m; i++){
        localres += A[data->indeks][i];
    }

    checkPthread(pthread_mutex_lock(&g_lock),"...");

    if(localres>maxres){
        maxres=localres;
        resind=data->indeks;
    }

    checkPthread(pthread_mutex_unlock(&g_lock),"...");
    return NULL;
}

int main(int argc, char **argv){

    scanf("%d",&m);
    A = malloc(m*sizeof(int*));
    checkError(A!=NULL,"malloc");
    for(int i=0; i<m; i++){
        A[i] = malloc(m*sizeof(int));
        checkError(A[i]!=NULL,"malloc");
    }

    for(int i=0; i<m; i++){
        for(int j=0; j<m; j++){
            scanf("%d",&A[i][i]);
        }
    }

    pthread_t *tids = malloc(m*sizeof(pthread_t));
    checkError(tids!=NULL,"malloc");
    InputArgs_t *data = malloc(m*sizeof(InputArgs_t));
    checkError(data!=NULL,"malloc");

    for(int i=0; i<m; i++){
        data[i].indeks = i;
        checkPthread(pthread_create(&tids[i],NULL,threadFunc,&data[i]),"pthread_create");
    }

    for(int i=0; i<m; i++)
        checkPthread(pthread_join(tids[i],NULL),"pthread_join");

    checkPthread(pthread_mutex_destroy(&g_lock),"pthread_mutex_destroy");

    printf("%d\n",resind);

    free(data);
    free(tids);
    for(int i=0; i<m; i++){
       free(A[i]);
    }
    free(A);

    exit(EXIT_SUCCESS);
}
