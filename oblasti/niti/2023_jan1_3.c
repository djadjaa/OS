#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <ctype.h>
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
        if(pthreadErr>0){ \
            errno = pthreadErr; \
            checkError(false,"usrMsg"); \
        } \
    }while(0); \

typedef struct{
    char *fpath;
    char c;
}Input;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int max=0, sumch=0;

//inicijalizujemo maksimalni karakter
char *maxch()  {
    static char ret = 0;
    return &ret;
  }

void *processThread(void *arg){
    Input *input = (void*)arg;
    FILE *f = fopen(input->fpath,"r");
    checkError(f!=NULL,"");

    char c;
    int localnumch=0;

    while((c=fgetc(f))!=EOF){
        if(tolower(c)==input->c)
            localnumch++;
    }

    pthreadCheckError(pthread_mutex_lock(&mutex),"");

    if(localnumch>max){
        max = localnumch;
        *maxch() = input->c;
    }
    sumch+=localnumch;

    pthreadCheckError(pthread_mutex_unlock(&mutex),"");
    fclose(f);
    return NULL;
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    //26 slova
    pthread_t tids[26];
    Input datainput[26];

    for(int i=0; i<26; i++){
        datainput[i].fpath = argv[1];
        datainput[i].c = i+'a';
        pthreadCheckError(pthread_create(&tids[i],NULL,processThread,&datainput[i]),"");
    }

    for(int i=0; i<26; i++)
        pthreadCheckError(pthread_join(tids[i],NULL),"");

    printf("%d %c %d\n",sumch,*maxch(),max);
    pthreadCheckError(pthread_mutex_destroy(&mutex),"");

    exit(EXIT_SUCCESS);
}