#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
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
    char *src = realpath(argv[1],NULL);
    char *dest = realpath(argv[2],NULL);
    checkError(src!=NULL,"");
    if(src!=NULL && dest!=NULL)
        checkError(strcmp(src,dest)!=0,"");
    free(src);
    free(dest);

    //src
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"");
    int fdsrc = open(argv[1],O_RDONLY);
    checkError(fdsrc!=-1,"");

    //dest
    mode_t stariUmask = umask(0);
    int fddest = open(argv[1], O_WRONLY|O_CREAT|O_TRUNC, fInfo.st_mode);
    checkError(fddest!=-1,"");

    umask(stariUmask);

    //ispis sa src na dest
    char *buff = malloc(MAX+1);
    checkError(buff!=NULL,"");

    int readBytes=0;
    while((readBytes=read(fdsrc,buff,MAX))>0)
        checkError(write(fddest,buff,readBytes)!=-1,"");
    checkError(readBytes!=-1,"");

    free(buff);
    close(fddest);
    close(fdsrc);

    exit(EXIT_SUCCESS);
}
