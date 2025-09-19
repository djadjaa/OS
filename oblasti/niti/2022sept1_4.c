#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define pthreadErrorCheck(err,usrMsg) \
    do{ \
        int pthreadErr = err; \
        if(pthreadErr > 0){ \
            errno = pthreadErr; \
            checkError(false,"usrMsg"); \
        } \
    }while(0); \

#define MAX 256
int n;
int numcharacters=0;
char res[MAX];
char **string; //matrica niski

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
    char *s;
}Input;

typedef struct{
    char stringres[MAX];
}Output;

void *processThread(void *arg){
    Input *input = (Input*)arg;
    char *tmpstring = input->s;
    int start = 0, end = strlen(tmpstring)-1;
    char tmp;
    while(start<end){
        tmp = tmpstring[start];
        tmpstring[start] = tmpstring[end];
        tmpstring[end] = tmp;

        start++;
        end--;
    }
    pthreadErrorCheck(pthread_mutex_lock(&mutex),"");
    numcharacters+=strlen(tmpstring);
    pthreadErrorCheck(pthread_mutex_unlock(&mutex),"");
    Output *output = malloc(sizeof(Output));
    checkError(output!=NULL,"");
    strcat(output->stringres,tmpstring);
    strcat(output->stringres," ");
    return output;
}

int main(int argc, char **argv){

    scanf("%d",&n);

    pthread_t *tids = malloc(n*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    
    Input *data_input = malloc(n*sizeof(Input));
    checkError(data_input!=NULL,"");

    string = (char**)malloc(n*sizeof(char*));
    checkError(string!=NULL,"");
    for(int i=0; i<n; i++){
        char *string = malloc(MAX*sizeof(char));
        checkError(string!=NULL,"");
        scanf("%s",string);
        data_input[i].s = string;
        pthreadErrorCheck(pthread_create(&tids[i],NULL,processThread,&data_input[i]),"");
    }

    for(int i=0; i<n; i++){
        Output *data_output = NULL;
        pthreadErrorCheck(pthread_join(tids[i],(void**)&data_output),"");
        strcat(res,data_output->stringres); //mogli smo i printf data_output->stringres svejedno
        free(data_output);
    }
    printf("%s%d\n",res,numcharacters);
    pthreadErrorCheck(pthread_mutex_destroy(&mutex),"");
    free(data_input);
    free(tids);
    for(int i=0; i<n; i++)
        free(string[i]);
    free(string);
    exit(EXIT_SUCCESS);
}
