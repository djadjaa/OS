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
float **mat;

typedef struct{
    int start;
}Input;

typedef struct{
    float min_sum;
}Output;

void *processThread(void *arg){
    Input *input = (Input*)arg;
    float min_local_sum = INT_MAX;
    float local_sum=0.0;
    for(int j=0; j<m; j++){
        local_sum += mat[input->start][j];
    }
    Output *output = malloc(sizeof(Output));
    checkError(output!=NULL,"");
    if(local_sum<min_local_sum)
        min_local_sum = local_sum;
    output->min_sum = min_local_sum;
    return output;

}

int main(int argc, char **argv){

    scanf("%d%d",&n,&m);
    mat = (float**)malloc(n*sizeof(float*));
    checkError(mat!=NULL,"");
    for(int i=0; i<n; i++){
        mat[i] = (float*)malloc(m*sizeof(float));
        checkError(mat[i]!=NULL,"");
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            scanf("%f",&mat[i][j]);
        }
    }
    pthread_t *tids = malloc(n*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    Input *data_input = malloc(n*sizeof(Input));
    checkError(data_input!=NULL,"");
    for(int i=0; i<n; i++){
        data_input[i].start = i;
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data_input[i]),"");
    }
    float res = INT_MAX;
    for(int i=0; i<n; i++){
        Output *data_output = NULL;
        osPthreadCheck(pthread_join(tids[i],(void**)&data_output),"");
        if(data_output->min_sum < res)
            res = data_output->min_sum;
        free(data_output);
    }
    printf("%f\n",res);
    free(tids);
    free(data_input);
    for(int i=0; i<n; i++)
        free(mat[i]);
    free(mat);
    exit(EXIT_SUCCESS);
}
