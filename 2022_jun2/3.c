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

typedef struct{
    int idx;
}ThreadArg;

int n;
int *a;
int globalsum=0;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void *processThread(void *args){
    ThreadArg *input = (ThreadArg*) args;
    int reverse=0,tmp;
    while(a[input->idx]!=0){
        tmp = a[input->idx]%10;
        reverse=reverse*10+tmp;
        a[input->idx]/=10;
    }
    
    osPthreadCheck(pthread_mutex_lock(&mutex),"");

    globalsum+=reverse;
    a[input->idx] = reverse;

    osPthreadCheck(pthread_mutex_unlock(&mutex),"");

    return NULL;

}

int main(int argc, char **argv){
    
    scanf("%d",&n);
    a = malloc(n*sizeof(int));
    checkError(a!=NULL,"");
    for(int i=0; i<n; i++)
        scanf("%d",&a[i]);
    pthread_t *tids = malloc(n*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    ThreadArg *data = malloc(n*sizeof(ThreadArg));
    checkError(data!=NULL,"");
    for(int i=0; i<n; i++){
        data[i].idx = i;
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data[i]),"");
    }
    for(int i=0; i<n; i++)
        osPthreadCheck(pthread_join(tids[i],NULL),"");
    for(int i=0; i<n; i++)
        printf("%d ",a[i]);
    printf("\n%d\n",globalsum);
    free(tids);
    free(data);
    free(a);
    exit(EXIT_SUCCESS);
}