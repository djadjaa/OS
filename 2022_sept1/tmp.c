#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include<string.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <pthread.h>

#define checkError(expr,userMsg) \
    do {\
        if (!(expr)) {\
            perror(userMsg);\
            exit(EXIT_FAILURE);\
        } \
    } while (0)

#define checkPthread(err,userMsg) \
    do { \
        if (err > 0) {\
            errno = err;\
            checkError(0,userMsg);\
        }\
    } while (0)

#define MAX_LEN (256)

// pthread_create
// pthread_join

typedef struct {
    char* niska;
} InputArgs_t;

typedef struct {
    char* retVal;
} OutputArgs_t;



int globalCounter = 0;
pthread_mutex_t gLock = PTHREAD_MUTEX_INITIALIZER;

void* threadFunc(void* arg) {

    InputArgs_t* iargs = (InputArgs_t*)arg;

    OutputArgs_t* oargs = malloc(sizeof(OutputArgs_t));


    char* localCounter = iargs->niska;
    char temp;

    int n = strlen(localCounter);

    int length = n-1;

   for(int i = 0; i < n/2; i++){

      temp = localCounter[i];

      localCounter[i] = localCounter[length];

      localCounter[length--] = temp;

   }

    checkPthread(pthread_mutex_lock(&gLock), "lock");

        globalCounter += n;

    checkPthread(pthread_mutex_unlock(&gLock), "unlock");

    oargs->retVal = localCounter;


    return oargs;
}

int main(int argc, char** argv) {

    checkError(argc == 1, "args");

     int n;
     scanf("%d",&n);


    pthread_t* tids = malloc(n*sizeof(pthread_t));
    checkError(tids != NULL, "malloc");

    InputArgs_t* args = malloc(n*sizeof(InputArgs_t));
    checkError(args != NULL, "malloc");

    for (int i = 0; i < n; i++) {

     char* s = malloc(MAX_LEN*sizeof(char));
     checkError(s != NULL, "malloc");

            scanf("%s",s);

        args[i].niska = s;

        checkPthread(pthread_create(&tids[i], NULL, threadFunc, &(args[i])), "create");
    }

    for (int i = 0; i < n; i++) {
        OutputArgs_t* retVal = NULL;
        checkPthread(pthread_join(tids[i], (void**)&retVal), "join");

        printf("%s\n",retVal->retVal);
        free(retVal);
    }

    free(tids);
    free(args);

    checkPthread(pthread_mutex_destroy(&gLock), "destroy");


    printf("%d\n",globalCounter);

    exit(EXIT_SUCCESS);
}