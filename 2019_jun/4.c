#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int fd = open(argv[1],O_RDONLY);
    checkError(fd!=-1,"open");
    FILE *f = fdopen(fd,"r");
    checkError(f!=NULL,"fdopen");

    char *line=NULL;
    size_t len;
    ssize_t readBytes;
    int offset=0;
    struct flock lock;
    while((readBytes=getline(&line,&len,f))!=-1){
        lock.l_type=F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = offset;
        lock.l_len = len; //ne menjamo rec pa necemo od pozadi citati

        checkError(fcntl(fd,F_GETLK,&lock)!=-1,"fcntl 1");

        if(lock.l_type==F_UNLCK)
            printf("%s",line);

        offset += readBytes; //drugi nacin preko ftell-a
    }

    free(line);
    fclose(f);

    exit(EXIT_SUCCESS);
}
