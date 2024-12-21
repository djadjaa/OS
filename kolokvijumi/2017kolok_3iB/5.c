#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

char *ext = NULL;

void processDir(const char *pname,int *numfiles){
    struct stat fInfo;
    checkError(stat(pname,&fInfo)!=-1,"stat");
    if(S_ISREG(fInfo.st_mode)){
        char *extmp = strrchr(pname,'.');
        if(extmp!=NULL && strcmp(ext,extmp)==0) //da li je ista ekstenzija
            (*numfiles)++;
    }else if(S_ISDIR(fInfo.st_mode)){
        DIR *dir = opendir(pname);
        checkError(dir!=NULL,"opendir");
        checkError(chdir(pname)!=-1,"chdir");
        struct dirent *entry;
        while((entry=readdir(dir))!=NULL){
            if(strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0)
                continue;
            processDir(entry->d_name,numfiles);
        }
        checkError(chdir("..")!=-1,"chdir"); //roditelj folder
        checkError(closedir(dir)!=-1,"closedir");
    }
}

/*
void processDir(char *pname,int *numfiles){
    DIR *dir = opendir(pname);
    checkError(dir!=NULL,"opendir");
    struct dirent *files;
    while((files=readdir(dir))!=NULL){
        if(strcmp(files->d_name,".")==0 || strcmp(files->d_name,"..")==0)
            continue;
        struct stat fInfo;
        checkError(stat(pname,&fInfo)!=-1,"stat");
        if(S_ISREG(fInfo.st_mode)){
            char* extmp = strrchr(pname,'.');
            if(extmp!=NULL && strcmp(ext,extmp)==0) //da li je ista ekstenzija
                (*numfiles)++;
        }
        processDir(files->d_name,numfiles);
    }
    checkError(closedir(dir)!=-1,"closedir");
}
*/
int main(int argc,char **argv){

    checkError(argc==3,"args");
    ext = argv[2];
    int numfiles=0;
    processDir(argv[1],&numfiles);
    printf("%d\n",numfiles);
    exit(EXIT_SUCCESS);
}