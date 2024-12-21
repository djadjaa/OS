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

    checkError(argc==3,"args");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"Open");
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");

    FILE *f_out = fopen(argv[2],"w");
    checkError(f_out,"fopen");

    char *line=NULL;
    size_t len=0;
    int num=0;

    while(getline(&line,&len,f)>0){
        struct flock lock;

        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = ftell(f);
        lock.l_len = -strlen(line);

        checkError(fcntl(fd,F_GETLK,&lock)!=-1,"fcntl");

        if(lock.l_type==F_UNLCK){
            fprintf(f_out,"%s",line);
            num++;
        }
    }
    printf("%d\n",num);
    fclose(f_out);
    fclose(f);
    exit(EXIT_SUCCESS);
}
