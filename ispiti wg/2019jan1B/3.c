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
#include <math.h>

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

int m,n,k;
float **vec=NULL;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
    int idx;
}InputData;

void *threadFunc(void *a){
    InputData *arg = (InputData*)a;
    float *localMax=malloc(sizeof(float)); //u adresu mu smeštamo rezultat
    checkError(localMax!=NULL,"malloc");
    *localMax=0;
    int i,j;
    for(i=0; i<m/k; i++) {
        float norma =0;
        for(j=0; j<n; j++){
            norma += vec[arg->idx*m/k+i][j] *vec[arg->idx*m/k+i][j];
        }
        norma = sqrtf(norma);
        if(norma>*localMax)
            *localMax=norma;
    }

    return (void*) localMax;
}

int main(int argc, char **argv){

    scanf("%d%d%d",&m,&n,&k);
    vec = malloc(m*sizeof(float*));
    checkError(vec!=NULL,"malloc");
    for(int i=0; i<m; i++){
        vec[i] = malloc(n*sizeof(float));
        checkError(vec[i]!=NULL,"malloc");
    }

    for(int i=0; i<m;i++){
        for(int j=0; j<n; j++){
            scanf("%f",&vec[i][j]);
        }
    }

    pthread_t *tids = malloc(k*sizeof(pthread_t));
    checkError(tids!=NULL,"malloc");
    InputData *data = malloc(k*sizeof(InputData));
    checkError(data!=NULL,"malloc");

    for(int i=0; i<k; i++){
        data[i].idx = i;
        checkPthread(pthread_create(&tids[i],NULL,threadFunc, &data[i])!=-1,"pthread_create");
    }

    int ind=0;
    float maxNorm=0;
    for(int i=0; i<k; i++){
        float *res;
        checkPthread(pthread_join(tids[i],(void**)&res)!=-1,"pthread_join");
        //uzeli smo rezultat sa funkcije threadFunc
        if(*res > maxNorm){
            maxNorm = *res;
            ind = i;
        }
        free(res);
    }
    printf("%d %f\n",ind,maxNorm);
    free(data);
    free(tids);

    for(int i=0; i<m; i++)
        free(vec[i]);
    free(vec);

    exit(EXIT_SUCCESS);
}
