#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

void processDir(char *path){
    DIR *directory = opendir(path);
    checkError(directory!=NULL,"directory");
    checkError(chdir(path)!=-1,"chdir");

    struct dirent *entry;
    while((entry=readdir(directory))!=NULL){
        if(strcmp(entry->d_name,"..")==0 || strcmp(entry->d_name,".")==0)
            continue;
        struct stat fInfo;
        checkError(stat(path,&fInfo)!=-1,"stat");

        //ulazimo u sledeci direktorijum
        char *newpath = malloc(strlen(path)+strlen(entry->d_name)+2);
        checkError(newpath!=NULL,"newpath");

        strcpy(newpath,path);
        strcat(newpath,"/");
        strcat(newpath,entry->d_name);

        if(S_ISDIR(fInfo.st_mode)){
            if(strstr(newpath,".git")!=NULL){
                char *res = realpath(path,NULL);
                printf("%s\n",res);
                free(res);
            }else{
                processDir(newpath);
            }
        }

        free(newpath);
        
    }

    checkError(chdir("..")!=-1,"chdir");
    checkError(closedir(directory)!=-1,"closedir");
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"notfolder");
    processDir(argv[1]);

    exit(EXIT_SUCCESS);
}