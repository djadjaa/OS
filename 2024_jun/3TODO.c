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

extern char **environ;

int main(int argc, char **argv){

    //uzimamo value i proveravamo koji od njih ima pravo citanja za sva tri
    char *value = getenv("PATH");
    //printf("%s\n",value);
    int size = strlen(value)+1;
    char *pathcopy = malloc(size*sizeof(char));
    checkError(pathcopy!=NULL,"malloc");
    strncpy(pathcopy,value,size);
    pathcopy[size+1]='\0';
    char *dir=strtok(pathcopy,":");
    while(dir!=NULL){

        struct stat fInfo;
        checkError(stat(dir,&fInfo)!=-1,"stat");
        if(S_ISDIR(fInfo.st_mode) && ((fInfo.st_mode & S_IRUSR) && (fInfo.st_mode & S_IRGRP) && (fInfo.st_mode & S_IROTH))){
            char *dirres = strrchr(dir,'/');
            if(dirres!=NULL)
                printf("%s ",dirres);
            else
                printf("%s ",dir);
        }

        dir=strtok(NULL,":");
    }

    exit(EXIT_SUCCESS);
}
