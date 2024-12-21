#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <limits.h>

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

#define KORAK 10

typedef struct{
    double x,y;
}Tacke;

typedef struct{
    int idx;
}InputData;

Tacke *points=NULL;
int n=0;
double globalMin=LONG_MAX;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void *threadFunc(void *a){
    InputData *arg = (InputData*)a;
    double localDist = 0, localMin=LONG_MAX;
    for(int i=0; i<n; i++){
        if(i==arg->idx)
            continue;
        //(tacke[i].x - tacke[pData->idx].x)*(tacke[i].x - tacke[pData->idx].x) + (tacke[i].y - tacke[pData->idx].y)*(tacke[i].y - tacke[pData->idx].y);
        localDist = ((points[i].x-points[arg->idx].x)*(points[i].x-points[arg->idx].x))+((points[i].y-points[arg->idx].y)*(points[i].y-points[arg->idx].y));
        if(localDist < localMin)
            localMin = localDist;
    }
    checkPthread(pthread_mutex_lock(&mut),"lock");

    if(localMin<globalMin)
        globalMin=localMin;

    checkPthread(pthread_mutex_unlock(&mut),"unlock");
    return NULL;
}

int main(int argc, char **argv){

    points = malloc(sizeof(Tacke));
    checkError(points!=NULL,"malloc");
    int alocirano=0,i=0;
    while(1){
        if(i==alocirano){
            alocirano += KORAK;
            points = realloc(points,sizeof(Tacke)*alocirano*2);
            checkError(points!=NULL,"realloc");
        }
        if(scanf("%lf%lf",&points[i].x,&points[i].y)!=2){
            break;
        }
        i++;
    }
    n=i;

    //pokrećemo n niti
    InputData *data = malloc(n*sizeof(InputData));
    checkError(data!=NULL,"malloc");
    pthread_t *tids = malloc(n*sizeof(pthread_t));
    checkError(tids!=NULL,"malloc");
    for(int i=0; i<n; i++){
        data[i].idx = i;
        checkError(pthread_create(&tids[i],NULL,threadFunc,&data[i])!=-1,"pthread_create");
    }
    for(int i=0; i<n; i++){
        checkPthread(pthread_join(tids[i],NULL),"join");
    }
    checkPthread(pthread_mutex_destroy(&mut)!=-1,"destroy");
    printf("%lf\n",sqrt(globalMin));
    free(tids);
    free(data);
    free(points);
    exit(EXIT_SUCCESS);
}
