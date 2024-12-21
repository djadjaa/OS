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
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"folder");
    char *oldpname = strrchr(argv[1],'/');
   // printf("%s\n",oldpname); old name = /c
    if(oldpname==NULL){
        checkError(rename(argv[1],argv[2])!=-1,"rename");
    }else{
        // a/b/c -> a/b/d
        
        int size = oldpname-argv[1]+1;
        //printf("%d\n",size); 4
        char *newpname = malloc(size+strlen(argv[2])+1);
        checkError(newpname!=NULL,"malloc");

        strncpy(newpname,argv[1],size);
        // printf("%s\n",newpname); a/b/
        strcat(newpname,argv[2]);
        //printf("%s\n",newpname); a/b/d
        checkError(rename(argv[1],newpname)!=-1,"rename");
        free(newpname);
    }

    exit(EXIT_SUCCESS);
}