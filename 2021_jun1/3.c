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

int m,n,k;
float **vector;

typedef struct{
    int idx;
}Input;

typedef struct{
    float max_norma;
}Output;

void *processThread(void *arg){
    Input *input = (Input*)arg;
    float localmaxnorma=0.0;
    for(int i=0; i<m/k; i++){
        float tempsum=0;
        for(int j=0; j<n; j++){
            tempsum += vector[i+input->idx][j]*vector[i+input->idx][j];
        }
        float tempsum2 = sqrt(tempsum);
        if(tempsum2>localmaxnorma){
            localmaxnorma = tempsum2;
        }
    }
    Output *output = malloc(sizeof(Output));
    checkError(output!=NULL,"");
    output->max_norma = localmaxnorma;
    return output;
}

int main(int argc, char **argv){

    scanf("%d%d%d",&m,&n,&k);
    vector = (float**)malloc(m*sizeof(float*));
    checkError(vector!=NULL,"");
    for(int i=0; i<m; i++){
        vector[i] = (float*)malloc(n*sizeof(float));
        checkError(vector[i]!=NULL,"");
    }
    for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){
            scanf("%f",&vector[i][j]);
        }
    }
    pthread_t *tids = malloc(k*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    Input *data_input = malloc(k*sizeof(Input));
    checkError(data_input!=NULL,"");
    for(int i=0; i<k; i++){
        data_input[i].idx = i*(m/k);
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data_input[i]),"");
    }
    float res = 0.0;
    int ind=0;
    for(int i=0; i<k; i++){
        Output *data_output = NULL;
        osPthreadCheck(pthread_join(tids[i],(void**)&data_output),"");
        if(data_output->max_norma > res){
            res = data_output->max_norma;
            ind = i;
        }
        free(data_output);
    }
    printf("%f %d\n",res,ind);
    free(tids);
    free(data_input);
    for(int i=0; i<m; i++)
        free(vector[i]);
    free(vector);
    exit(EXIT_SUCCESS);
}
