#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define pthreadCheckError(err,usrMsg) \
    do{ \
        int pthreadErr = err; \
        if(pthreadErr>0){ \
            errno = pthreadErr; \
            checkError(0,"usrMsg"); \
        } \
    }while(0); \

int n,m;
double **mat;
double globalmax=INT_MIN;
int globalrowmax = 0, globalcolmax = 0;

typedef struct{
    int jdx;
}Input;

typedef struct{
    double res;
    int rowmax,colmax;
}Output;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *processThread(void *arg){
    Input *input = (Input*) arg;

    Output *output = malloc(sizeof(Output));
    checkError(output!=NULL,"");

    //inicijalizacija
    float localmax = INT_MIN;
    int localrowmax = 0;
    int localcolmax = input->jdx;

    for(int i=0; i<n; i++){
        if(mat[i][input->jdx] > localmax){
            localmax = mat[i][input->jdx];
            localrowmax = i;
            //localcolmax = input->jdx; nije neophodno
        }
    }
    //povratna vrednost kroz niti
    output->res = localmax;
    output->rowmax = localrowmax;
    output->colmax = localcolmax;

    pthreadCheckError(pthread_mutex_lock(&mutex),""); 
    //povratna vrednost kroz globalne maksimume koje ispisujemo na standardni izlaz
    if (localmax > globalmax) {
        globalmax = localmax;
        globalrowmax = localrowmax;
        globalcolmax = localcolmax;
    }

    pthreadCheckError(pthread_mutex_unlock(&mutex),"");    

    return output; //moze i pthread_exit(output);
}

int main(int argc, char **argv){

    scanf("%d%d",&n,&m);

    pthread_t *tids = malloc(m*sizeof(pthread_t));
    checkError(tids!=NULL,"");

    Input *datainput = malloc(m*sizeof(Input));
    checkError(datainput!=NULL,"");

    mat = malloc(n*sizeof(double*));
    checkError(mat!=NULL,"");
    for(int i=0; i<n; i++){
        mat[i] = malloc(m*sizeof(double));
        checkError(mat[i]!=NULL,"");
        for(int j=0; j<m; j++){
            scanf("%lf",&mat[i][j]);
        }
    }

    for(int j=0; j<m; j++){
        datainput[j].jdx = j;
        pthreadCheckError(pthread_create(&tids[j],NULL,processThread,&datainput[j]),"");
    }

    for(int j=0; j<m; j++){
        Output *dataoutput = NULL;
        pthreadCheckError(pthread_join(tids[j],(void**)&dataoutput),"");
        /*
        globalmax = dataoutput->res;
        globalrowmax = dataoutput->rowmax;
        globalcolmax = dataoutput->colmax;
        printf("%lf %d %d\n",globalmax,globalrowmax,globalcolmax);*/
        free(dataoutput);
    }

    printf("%.lf %d %d\n",globalmax,globalrowmax,globalcolmax);
    free(tids);
    free(datainput);
    for(int i=0; i<n; i++)
        free(mat[i]);
    free(mat);
    exit(EXIT_SUCCESS);
}
