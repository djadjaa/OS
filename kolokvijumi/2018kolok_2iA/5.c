#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

void processDir(char *pname, unsigned *min, unsigned *max){
    struct stat fInfo;
    checkError(stat(pname,&fInfo)!=-1,"stat");
    if(S_ISREG(fInfo.st_mode)){
        if(fInfo.st_size < *min)
            *min = fInfo.st_size;
        else if(fInfo.st_size > *max)
            *max = fInfo.st_size;
    }else if(S_ISDIR(fInfo.st_mode)){
        DIR *dir = opendir(pname);
        checkError(dir!=NULL,"opendir");
        checkError(chdir(pname)!=-1,"chdir");
        struct dirent *entry;
        while(entry = readdir(dir)){
            if(strcmp(".",entry->d_name)==0 || strcmp("..",entry->d_name)==0)
                continue;
            processDir(entry->d_name,min,max);
        }
        checkError(chdir("..")!=-1,"chdir");
        checkError(closedir(dir)!=-1,"closedir");
    }
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"folder");
    unsigned minsize=0XFFFFFFFFU,maxsize=0;
    processDir(argv[1],&minsize,&maxsize);
    printf("%u\n",maxsize-minsize);

    exit(EXIT_SUCCESS);
}