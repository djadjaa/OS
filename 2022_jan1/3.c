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

#define MAX 256

typedef struct{
    char *fpath;
}Input;

typedef struct{
    int numwrd;
}Output;

char *fpathres=NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *processThread(void *arg){
    Input *input = (Input*)arg;
    FILE *f = fopen(input->fpath,"r");
    checkError(f!=NULL,"");

    int localmaxwrd=0, numwrdtmp=0;
    char buff[MAX];
    while(fscanf(f,"%s",buff)==1)
        numwrdtmp++;

    fclose(f);

    osPthreadCheck(pthread_mutex_lock(&mutex),"");

    if(numwrdtmp>localmaxwrd){
        localmaxwrd=numwrdtmp;
        fpathres = input->fpath;
    }

    osPthreadCheck(pthread_mutex_unlock(&mutex),"");

    Output *output = malloc(sizeof(Output)); //ne smemo odrediti max br reci u mainu vec preko niti
    checkError(output!=NULL,"");
    output->numwrd = numwrdtmp;
    return output;

}

int main(int argc, char **argv){

    checkError(argc>2,"args");
    int numThreads=argc-1;
    pthread_t *tids = malloc(numThreads*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    Input *data_input = malloc(numThreads*sizeof(Input));
    checkError(data_input!=NULL,"");
    for(int i=0; i<numThreads; i++){
        struct stat fInfo;
        checkError(stat(argv[i+1],&fInfo)!=-1,"");
        checkError(S_ISREG(fInfo.st_mode),"not reg");
        data_input[i].fpath = argv[i+1];
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data_input[i]),"");
    }
    int ukreci=0;
    for(int i=0; i<numThreads; i++){
        Output *data_output = NULL;
        osPthreadCheck(pthread_join(tids[i],(void**)&data_output),"");
        ukreci+=data_output->numwrd;
        free(data_output);
    }
    char* res = strrchr(fpathres, '/');
    printf("%d %s\n",ukreci,(res != NULL ? res + 1 : fpathres));
    free(tids);
    free(data_input);
    exit(EXIT_SUCCESS);
}