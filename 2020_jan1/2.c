//TODO izmeniti samo tip u double/long

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

#define osPthreadCheck(err,usrMsg)\
    do{\
        int pthreadErr = err;\
        if(pthreadErr>0){\
            errno = pthreadErr;\
            checkError(false,"usrMsg");\
        }\
    }while(0);

int m,n;
//upotreba povratnih vrednosti niti
float **mat;

typedef struct{
    int start;
}Input;

typedef struct{
    float max_product;
}Output;

void *processThread(void *arg){
    Input *input = (Input*)arg;
    float max_local_product=INT_MIN;
    float local_product = 1.0;
    for(int j=0; j<m; j++)
        local_product *= mat[input->start][j];
    //printf("%f\n",local_product);
    Output *output = (Output*)malloc(sizeof(Output));
    checkError(output!=NULL,"");
    if(local_product>max_local_product){
        max_local_product=local_product;
    }
    output->max_product = max_local_product;
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
    pthread_t *tids=malloc(n*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    Input *data_input = malloc(n*sizeof(Input));
    checkError(data_input!=NULL,"");

    for(int i=0; i<n; i++){
        data_input[i].start = i;
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data_input[i]),"");
    }
    float res = INT_MIN;
    for(int i=0; i<n; i++){
        Output *data_output = NULL;
        osPthreadCheck(pthread_join(tids[i],(void**)&data_output),"");
        if(data_output->max_product > res)
            res = data_output->max_product;
        free(data_output);
    }
    printf("%f\n",res);
    free(data_input);
    free(tids);
    for(int i=0; i<n; i++)
        free(mat[i]);
    free(mat);
    exit(EXIT_SUCCESS);
}
