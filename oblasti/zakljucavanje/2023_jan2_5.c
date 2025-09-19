#define _POSIX_C_SOURCE 2
#define _XOPEN_SOURCE 700
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

int main(int argc, char **argv){

    checkError(argc==3,"");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"");
    FILE *f_in = fdopen(fd,"r+");
    checkError(f_in!=NULL,"");

    FILE *f_out = fopen(argv[2],"w");
    checkError(f_out!=NULL,"");

    char *line=NULL;
    size_t len=0;
    int numUnlocked=0;

    while(getline(&line,&len,f_in)>0){
        struct flock lock;

        lock.l_len = -strlen(line);
        lock.l_start = ftell(f_in);
        lock.l_whence = SEEK_SET;
        lock.l_type = F_WRLCK;

        checkError(fcntl(fd,F_GETLK,&lock)!=-1,"");

        if(lock.l_type==F_UNLCK){
            fprintf(f_out,"%s",line);
            numUnlocked++;
        }
    }
    printf("%d\n",numUnlocked);
    fclose(f_out);
    fclose(f_in);
    exit(EXIT_SUCCESS);
}