#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){

    checkError(argc>=2,"args");
    struct stat fInfo[argc];
    for(int i=1; i<argc; i++){
        checkError(stat(argv[i],&fInfo[i])!=-1,"stat");
        checkError(S_ISDIR(fInfo[i].st_mode),"folder"); //prima relativne putanje do direktorijuma
        if(!(fInfo[i].st_mode & S_IROTH) && !(fInfo[i].st_mode & S_IWOTH) && !(fInfo[i].st_mode & S_IXOTH)){
            char *path = realpath(argv[i],NULL);
            checkError(path!=NULL,"path");
            printf("%d\n",(int)strlen(path));
            free(path);
        }
    }

    exit(EXIT_SUCCESS);
}