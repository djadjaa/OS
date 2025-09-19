#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define pthreadCheckError(err, usrMsg) \
    do{ \
        int pthreadErr = err; \
        if(pthreadErr>0) { \
            errno = pthreadErr; \
            checkError(0,"usrMsg"); \
        } \
    }while(0); \

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
    Input *input = (Input*) arg;
    FILE *f = fopen(input->fpath,"r");
    checkError(f!=NULL,"");

    int localmaxwrd = 0, numwrdtmp = 0;
    char buff[MAX];
    //brojimo reci
    while(fscanf(f,"%s",buff)==1)
        numwrdtmp++;
    fclose(f);

    pthreadCheckError(pthread_mutex_lock(&mutex),"");

    if(numwrdtmp > localmaxwrd){
        localmaxwrd = numwrdtmp;
        fpathres = input->fpath;
    }

    pthreadCheckError(pthread_mutex_unlock(&mutex),"");

    Output *output = malloc(sizeof(Output));
    checkError(output!=NULL,"");
    output->numwrd = numwrdtmp;
    return output;
}

int main(int argc, char **argv){

    checkError(argc>2,"");
    int numthreads = argc-1;

    pthread_t *tids = malloc(numthreads*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    Input *inputdata = malloc(numthreads*sizeof(Input));
    checkError(inputdata!=NULL,"");

    for(int i=0; i<numthreads; i++){
        struct stat fInfo;
        checkError(stat(argv[i+1],&fInfo)!=-1,"");
        checkError(S_ISREG(fInfo.st_mode),"");
        inputdata[i].fpath = argv[i+1];
        pthreadCheckError(pthread_create(&tids[i],NULL,processThread,&inputdata[i]),"");
    }
    int globalsumwrds=0;
    for(int i=0; i<numthreads; i++){
        Output *dataoutput = NULL;
        pthreadCheckError(pthread_join(tids[i],(void**)&dataoutput),"");
        globalsumwrds += dataoutput->numwrd;
        free(dataoutput);
    }
    char* res = strrchr(fpathres, '/');
    printf("%d %s\n",globalsumwrds,(res != NULL ? res + 1 : fpathres));
    exit(EXIT_SUCCESS);
}
