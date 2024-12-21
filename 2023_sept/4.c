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
#include <ctype.h>
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

int n,globalsum=0;
int *a;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
    int idx;
}Input;

typedef struct{
    int sum;
}Output;

void *processThread(void *args){
    Input *input = (Input*)args;
    int tmp=a[input->idx];
    int tmpsum=0;
    while(tmp!=0){
        tmpsum+=(tmp%10);
        tmp/=10;
    }
    osPthreadCheck(pthread_mutex_lock(&mutex),"");
    
    globalsum += tmpsum;
    osPthreadCheck(pthread_mutex_unlock(&mutex),"");

    Output *output = malloc(sizeof(Output));
    checkError(output!=NULL,"");

    output->sum = globalsum;
    return output;
}

int main(int argc, char **argv){

    scanf("%d",&n);
    a=malloc(n*sizeof(int));
    checkError(a!=NULL,"");
    for(int i=0; i<n; i++)
        scanf("%d",&a[i]);
    pthread_t *tids = malloc(n*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    Input *data_input= malloc(n*sizeof(pthread_t));
    checkError(data_input!=NULL,"");
    for(int i=0; i<n; i++){
        data_input[i].idx = i;
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data_input[i]),"");
    }
    for(int i=0; i<n; i++){
        Output *data_output = NULL;
        osPthreadCheck(pthread_join(tids[i],(void**)&data_output),"");
        globalsum = data_output->sum;
        free(data_output);
    }
    printf("%d\n",globalsum);
    free(tids);
    free(data_input);
    free(a);
    osPthreadCheck(pthread_mutex_destroy(&mutex),"");
    exit(EXIT_SUCCESS);
}
