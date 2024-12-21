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

#define MAX 2048

typedef struct{
    char *path_th;
    int start,len;
}ThreadArg;

int kodoni[MAX];
int duzina=0;
char *kodon1="tag";
char *kodon2="taa";
char *kodon3="tga";
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void *processThread(void *args){
    ThreadArg *input = (ThreadArg*)args;

    int fd = open(input->path_th,O_RDONLY);
    checkError(fd!=-1,"open");

    //uzimanje podataka za citanje
    checkError(lseek(fd,input->start,SEEK_SET)!=-1,"");

    char buff[input->len+1];
    //citanje
    checkError(read(fd,buff,input->len)!=-1,"");
    buff[input->len]='\0';

    int pozicije[MAX];
    int p=0;
    for(int i=0; i<input->len; i++){
        //proveravamo je l imaju kodone u sebi
        if(strncmp(&buff[i],kodon1,3)==0 || strncmp(&buff[i],kodon2,3)==0 || strncmp(&buff[i],kodon3,3)==0){
            pozicije[p++] = input->start+i;
        }
    }
    osPthreadCheck(pthread_mutex_lock(&mutex),"");
    //za svako pojavljivanje jedne od ovih niski dodaju poziciju pocetka u globalni niz 
    for(int i=0; i<p; i++)
        kodoni[duzina++] = pozicije[i];
    osPthreadCheck(pthread_mutex_unlock(&mutex),"");
    close(fd);

    return NULL;
}

int main(int argc, char **argv){
    
    checkError(argc==3,"args");
    int n = strtol(argv[1],NULL,10);

    struct stat fInfo;
    checkError(stat(argv[2],&fInfo)!=-1,"");
    char *path = argv[2];
    int m = fInfo.st_size; //size of file

    pthread_t *tids = malloc(n*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    ThreadArg *data = malloc(n*sizeof(ThreadArg));
    checkError(data!=NULL,"");

    for(int i=0; i<n; i++){
        data[i].path_th = path;
        data[i].start = i*(m/n);
        data[i].len = m/n;
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data[i]),"");
    }
    for(int i=0; i<n; i++)
        osPthreadCheck(pthread_join(tids[i],NULL),"");
    for(int i=0; i<duzina; i++)
        printf("%d ",kodoni[i]); //ispisujemo pozicije gde se nalaze kodoni
    putchar('\n');
    free(tids);
    free(data);
    exit(EXIT_SUCCESS);
}