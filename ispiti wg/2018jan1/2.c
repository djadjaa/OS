#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

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
    char *path_th;
    char *w_th;
    int start,len;
}Input;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
int res=0;

void *processThread(void *args){
    Input *input = (Input*) args;

    int fd = open(input->path_th,O_RDONLY);
    checkError(fd!=-1,"");

    checkError(lseek(fd,input->start,SEEK_SET)!=-1,""); //izdvajamo koliko citamo

    char buff[input->len+1];
    checkError(read(fd,buff,input->len)!=-1,""); //citamo zvanicno

    buff[input->len]='\0';
    close(fd);

    int sum=0;
    char *s=buff;
    while((s=strstr(s,input->w_th))!=NULL){ //izvlacimo podstring dokle god postoji
        sum++;
        s++;
    }

    osPthreadCheck(pthread_mutex_lock(&mutex),"");

    res +=sum;

    osPthreadCheck(pthread_mutex_unlock(&mutex),"");


    return NULL;
}

int main(int argc, char **argv){
    
    checkError(argc==4,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"");
    int n=fInfo.st_size;
    char *path = argv[1];
    char *w = argv[2];
    int k = strtol(argv[3],NULL,10);

    pthread_t *tids = malloc(k*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    Input *data_input=malloc(k*sizeof(Input));
    checkError(data_input!=NULL,"");

    for(int i=0; i<k; i++){
        data_input[i].path_th=path;
        data_input[i].w_th=w;
        data_input[i].start=i*(n/k);
        data_input[i].len=n/k;
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data_input[i]),"");
    }
    for(int i=0; i<k; i++)
        osPthreadCheck(pthread_join(tids[i],NULL),"");
    printf("%d\n",res);
    free(tids);
    free(data_input);
    exit(EXIT_SUCCESS);
}