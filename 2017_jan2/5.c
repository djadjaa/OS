#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 256

int main(int argc, char **argv){

    checkError(argc==3,"");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"");
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"");
    char *wrd = argv[2];
    char buff[MAX];
    while(fscanf(f,"%s",buff)==1){
        if(strcmp(buff,wrd)==0){
            struct flock lock;
            lock.l_type=F_WRLCK;
            lock.l_len=-strlen(buff);
            lock.l_start=ftell(f);
            lock.l_whence=SEEK_SET;
            checkError(fcntl(fd,F_GETLK,&lock)!=-1,"");
            //printf("%ld ",ftell(f)-strlen(wrd)); ne mozemo ovde jer ce ispisati poziciju kada je otkljucana rec
            switch (lock.l_type){
                case F_WRLCK:
                    printf("%ld w\n",ftell(f)-strlen(wrd));
                    break;
                case F_RDLCK:
                    printf("%ld r\n",ftell(f)-strlen(wrd));
                    break;
                case F_UNLCK:
                    break;
                }
        }
    }
    fclose(f);
    exit(EXIT_SUCCESS);
}