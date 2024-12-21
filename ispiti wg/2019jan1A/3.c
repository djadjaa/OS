/*
Sa standardnog ulaza se ucitavaju celi brojevi m, n i k pa zatim m+1 vektor duzine n (elementi vektora su realni brojevi jednostruke tacnosti tj. float). 
Potrebno je pokrenuti k niti od kojih svaka racuna skalarni proizvod m/k vektora sa poslednje ucitanim vektorom (predpostaviti m). 
Niti vracaju maksimalan lokalni skalarni proizvod kao povratnu vrednost koja se dohvata u main funkciji. 
Ispisati iz main funkcije indeks niti koja je izracunala najveci skalarni proizvod medju svim nitima, kao i vrednost tog skalarnog proizvoda. 
Maksimum skalarnih proizvoda koje su niti vratile mora da se racuna u main funkciji!
*/

#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>

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

int m,n,k;
float **vectors;

typedef struct{
    int start;
}Input;

typedef struct{
    float local_max_th;
}Output;

void *processThread(void *args){

    Input *input = (Input*)args;
    float local_max = -1.0;
    for(int i=0; i<m/k; i++){
        float skalarnipr=0.0;
        for(int j=0; j<n; j++){
            skalarnipr += vectors[i+input->start][j]*vectors[m][j];
        }
        if(skalarnipr>local_max)
            local_max = skalarnipr;
    }
    Output *output = (Output*)malloc(sizeof(Output));
    checkError(output!=NULL,"");
    output->local_max_th = local_max;
    return output; //vracamo nit preko koje prosledjujemo rezultat
}

int main(int argc, char **argv){
    
    scanf("%d%d%d",&m,&n,&k);
    vectors = (float**)malloc((m+1)*sizeof(float*));
    checkError(vectors!=NULL,"Malloc");
    for(int i=0; i<=m; i++){
        vectors[i]=(float*)malloc(n*sizeof(float));
        checkError(vectors[i]!=NULL,"");
    }
    for(int i=0; i<=m; i++){
        for(int j=0; j<n; j++){
            scanf("%f",&vectors[i][j]);
        }
    }
    pthread_t *tids =(pthread_t *)malloc(k*sizeof(pthread_t));
    checkError(tids!=NULL,"");
    Input *data_input = (Input *)malloc(k*sizeof(Input));
    checkError(data_input!=NULL,"");
    for(int i=0; i<k; i++){
        data_input[i].start = i*(m/k);
        osPthreadCheck(pthread_create(&tids[i],NULL,processThread,&data_input[i]),"");
    }
    float max = -1.0;
    int num_threads=-1;
    for(int i=0; i<k; i++){
        Output *data_output=NULL;
        osPthreadCheck(pthread_join(tids[i],(void**)&data_output),""); //2. arg je gde vracamo rez
        if(data_output->local_max_th > max){
            max =data_output->local_max_th;
            num_threads=i+1;
        }
        free(data_output);
    }
    printf("%d %f\n",num_threads,max);
    free(data_input);
    free(tids);
    for(int i=0; i<=m; i++)
        free(vectors[i]);
    free(vectors);
    exit(EXIT_SUCCESS);
}