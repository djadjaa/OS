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
    float max_skalarpr;
}Output;

void *processThread(void *arg){
    Input *input = (Input*)arg;
    float local_max_skalarpr = -1.0;
    for(int i=0; i<m/k; i++){
        float skalarpr = 0.0;
        for(int j=0; j<n; j++){
            skalarpr += vector[i+input->idx][j]*vector[m][j];
        }
        if(skalarpr>local_max_skalarpr)
            local_max_skalarpr=skalarpr;
    }
    Output *output = malloc(sizeof(Output));
    checkError(output!=NULL,"");
    output->max_skalarpr = local_max_skalarpr;
    return output;
}

int main(int argc, char **argv){

    scanf("%d%d%d",&m,&n,&k);
    vector = (float**)malloc(m*sizeof(float*));
    checkError(vector!=NULL,"");
    for(int i=0; i<=m; i++){
        vector[i]=(float*)malloc(n*sizeof(float));
        checkError(vector[i]!=NULL,"");
    }
    for(int i=0; i<=m; i++){
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
    float res=-1.0;
    int ind=0;
    for(int i=0; i<k; i++){
        Output *data_output = NULL;
        osPthreadCheck(pthread_join(tids[i],(void**)&data_output),"");
        if(data_output->max_skalarpr>res){
            res = data_output->max_skalarpr;
            ind = i+1; //imamo m+1 vektora
        }
        free(data_output);
    }
    printf("%d %.1f\n",ind,res);
    free(tids);
    free(data_input);
    for(int i=0; i<=m; i++)
        free(vector[i]);
    free(vector);
    exit(EXIT_SUCCESS);
}
