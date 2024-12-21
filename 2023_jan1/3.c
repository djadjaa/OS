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

typedef struct{
    char *fpath;
    char c;
    int idx;
}ThreadArgs;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int maxnumch=0;
//char *maxch;
int uknumch=0;

//pravimo funkciju tipa char* bez argumenata koja vraca adresu rezultata koji je prethodno inicijalizovan na 0
char *maxch()  {
  static char ret = 0;
  return &ret;
}

void *processThread(void *arg){
    ThreadArgs *input = (ThreadArgs*)arg;
    FILE *f  = fopen(input->fpath,"r");
    checkError(f!=NULL,"");

    char c;
    int numch=0;

    while((c=fgetc(f))!=EOF){
        if(tolower(c)==input->c)
            numch++;
    }

    osPthreadCheck(pthread_mutex_lock(&mutex),"");

    if(numch>maxnumch){
        maxnumch=numch;
        *maxch() = input->c; //dodeljujemo rezultat
    }
    uknumch+=numch;

    osPthreadCheck(pthread_mutex_unlock(&mutex),"");

    fclose(f);
    return NULL;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");

    pthread_t tids[26];
    ThreadArgs data[26];

    for(int i=0; i<26; i++){
        data[i].idx = i;
        data[i].fpath = argv[1];
        data[i].c = i+'a';
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data[i]),"");
    }
    
    for(int i=0; i<26; i++)
        osPthreadCheck(pthread_join(tids[i],NULL),"");
    
    printf("%d %c %d\n",uknumch,*maxch(),maxnumch);

    osPthreadCheck(pthread_mutex_destroy(&mutex),"");
    exit(EXIT_SUCCESS);
}
