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

int n,m;
float **mat;

typedef struct{
    int column;
}Input;

typedef struct{
    float max;
    int maxrow,maxcol;
}Output;

float globalmax=INT_MIN;
int resrow=0, rescol=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *processThread(void *arg){
    Input *input = (Input*)arg;

    /*
    float localmax=INT_MIN;
    int localresrow=0, localrescol=0;
    */
    osPthreadCheck(pthread_mutex_lock(&mutex),"");
    for(int i=0; i<n; i++){
        if(mat[i][input->column] > globalmax){
            globalmax = mat[i][input->column];
            resrow = i;
            rescol = input->column;
        }
        //printf("%.2f %d %d\n",localmax,localresrow,localrescol);
    }
    osPthreadCheck(pthread_mutex_unlock(&mutex),"");
    
    /*
    
    Output *output = malloc(sizeof(Output));
    checkError(output!=NULL,"");
    output->max = globalmax;
    output->maxrow = resrow;
    output->maxcol = rescol;
    */
    return NULL;
}

int main(int argc, char **argv){

    scanf("%d%d",&n,&m);
    mat = malloc(n*sizeof(float*));
    checkError(mat!=NULL,"");
    for(int i=0; i<n; i++){
        mat[i] = malloc(m*sizeof(float));
        checkError(mat[i]!=NULL,"");
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            scanf("%f",&mat[i][j]);
        }
    }
    pthread_t *tids = malloc(m*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    Input *data_input =malloc(m*sizeof(pthread_t));
    checkError(data_input!=NULL,"");
    for(int j=0; j<m; j++){
        data_input[j].column = j;
        osPthreadCheck(pthread_create(&tids[j],NULL,processThread,&data_input[j]),"");
    }
    for(int j=0; j<m; j++){
        /*
        
        Output *data_output = NULL;
        osPthreadCheck(pthread_join(tids[j],(void**)&data_output),"");
        globalmax = data_output->max;
        resrow = data_output->maxrow;
        rescol = data_output->maxcol; 
        free(data_output);
         */

        
        osPthreadCheck(pthread_join(tids[j],NULL),"");
    }
    printf("%.2f %d %d\n",globalmax,resrow,rescol);
    free(tids);
    free(data_input);
    for(int i=0; i<n; i++)
        free(mat[i]);
    free(mat);
    exit(EXIT_SUCCESS);
}
