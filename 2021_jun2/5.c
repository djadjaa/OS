#define _POSIX_C_SOURCE 2
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 256
int main(int argc, char **argv){

    checkError(argc==3,"args");
    int lenwrd = strlen(argv[2]);
    int fd = open(argv[1],O_RDWR); //za fcntl
    checkError(fd!=-1,"open");

    FILE *f = fdopen(fd,"r");
    checkError(f!=NULL,"fdopen");

    char buff[MAX];

    //citamo rec po rec i zakljucavamo rec
    while(fscanf(f,"%s",buff)==1){
        if(strcmp(buff,argv[2])==0){ //ako su jednake zakljucavamo
            struct flock lock;
            lock.l_type = F_WRLCK; //ili RDLCK svejedno
            lock.l_whence = SEEK_SET; //citamo od pocetka fajla
            lock.l_start = ftell(f); //tekuci fajl offst -> zakljucava se u odnosu na poslednju procitanu pozciju pomocu fja iz stdio
            lock.l_len = -lenwrd; //zakljucavamo unazad relativno u odnosu na trenutni polozaj ako treba zameniti procitanu rec nekom drugom

            checkError(fcntl(fd,F_GETLK, &lock)!=-1,"fcntl");

            if(lock.l_type==F_WRLCK){
                printf("%ld w\n",ftell(f)-lenwrd);
            }else if(lock.l_type==F_RDLCK){
                printf("%ld r\n",ftell(f)-lenwrd);
           }
        }
    }

    fclose(f);

    exit(EXIT_SUCCESS);
}
