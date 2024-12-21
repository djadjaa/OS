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

#define MAX 256

int globalsum=0;
char *paths;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *processThread(void *args){
    FILE *f = fopen(paths,"r");
    checkError(f!=NULL,"");

    char buff[MAX];
    int localsum=0;

    while (fscanf(f,"%s",buff)){
        localsum += strlen(buff);
    }

    osPthreadCheck(pthread_mutex_lock(&mutex),"");
    globalsum += localsum;
    osPthreadCheck(pthread_mutex_unlock(&mutex),"");

    fclose(f);
    return NULL;
}

int main(int argc, char **argv){
    while(scanf("%ms",&paths)!=EOF){
        pthread_t backgroundThread;
        osPthreadCheck(pthread_create(&backgroundThread,NULL,processThread,NULL),"");
        osPthreadCheck(pthread_join(backgroundThread,NULL),"");
    }
    printf("%d\n",globalsum);
   // osPthreadCheck(pthread_detach(backgroundThread),"");
    osPthreadCheck(pthread_mutex_destroy(&mutex),"");
    exit(EXIT_SUCCESS);
}
