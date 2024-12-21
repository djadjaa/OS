#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int num=0;

void processDir(const char *fpath, char *s, int increment){
    struct stat fInfo;
    checkError(stat(fpath,&fInfo)!=-1,"stat");
    if(increment)
        num++;
    if(!S_ISDIR(fInfo.st_mode))
        return;
    DIR *directory = opendir(fpath);
    checkError(directory!=NULL,"Opendir");
    struct dirent *entry;
    while((entry=readdir(directory))!=NULL){
        if(!strcmp(entry->d_name,".") || !strcmp(entry->d_name,".."))
            continue;
        
        char *tmp = malloc((strlen(fpath)+strlen(entry->d_name)+2)*sizeof(char));
        checkError(tmp!=NULL,"malloc");
        strcpy(tmp,fpath);
        strcat(tmp,"/");
        strcat(tmp,entry->d_name);

        if(increment)
            processDir(tmp,s,1);
        else if(strstr(tmp,s)==NULL)
            processDir(tmp,s,1);
        else
            processDir(tmp,s,0);
        free(tmp);
    }
    checkError(closedir(directory)!=-1,"closedir");
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"not dir");
    processDir(argv[1],argv[2],0);
    printf("%d\n",num);
    exit(EXIT_SUCCESS);
}
