#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define pthreadCheckError(err,userMsg) \
    do { \
        int _err = err; \
        if (_err > 0) { \
            errno = _err; \
            checkError(0,userMsg); \
        } \
    } while (0)

#define MAX 256

typedef struct{
    char *s;
}Input;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int sumch=0;

void *processThread (void *arg){
    Input *input = (void*)arg;

    int localsumch=strlen(input->s);
    
    pthreadCheckError(pthread_mutex_lock(&mutex),"lock");

    sumch += localsumch;

    pthreadCheckError(pthread_mutex_unlock(&mutex),"unlock");

    return NULL;
}

int main(int argc, char **argv){

    char *pathname;
    while(scanf("%ms",&pathname)!=EOF){
        FILE *f = fopen(pathname,"r+");
        checkError(f!=NULL,"fopen");
        char buff1[MAX],buff2[MAX];
        int n=0;
        //uzimamo br reci
        while(fscanf(f,"%s",buff1)==1){
            n++;
        }
        
        rewind(f); //ili fseek(f, 0, SEEK_SET);
        pthread_t *tids = malloc(n*sizeof(pthread_t));
        checkError(tids!=NULL,"malloc tids");
        Input *datainput = malloc(n*sizeof(Input));
        checkError(datainput!=NULL,"data input malloc");
        
        //sada broj niti
        int count=0;
        while(fscanf(f,"%s",buff2)==1 && count<n){
            datainput[count].s = malloc(strlen(buff2)+1);
            checkError(datainput[count].s!=NULL,"");
            strcpy(datainput[count].s,buff2);
            pthreadCheckError(pthread_create(&tids[count],NULL,processThread,&datainput[count]),"pthread create");
            count++;
        }

        for(int i=0; i<count; i++){
            pthreadCheckError(pthread_join(tids[i],NULL),"pthread join");
            free(datainput[i].s);
        }
            
        free(tids);
        free(datainput);
        free(pathname);
    }
    printf("%d\n",sumch);

    exit(EXIT_SUCCESS);
}
