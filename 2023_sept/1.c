#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ftw.h>
#include <string.h>
#include <dirent.h>

#define checkError(expr,userMsg) \
    do { \
        if(!(expr)){ \
            perror(userMsg); \
            exit(1); \
        } \
    }while(0)

void howManyGits(const char *pathname){
    DIR *directory = opendir(pathname);
    checkError(directory!=NULL,"opendir");

    struct dirent *fajl;
    while ((fajl=readdir(directory))!=NULL) {
        if(strcmp(fajl->d_name, ".")==0 || strcmp(fajl->d_name,"..")==0)
            continue;
        struct stat fileInfo;
        checkError(stat(pathname,&fileInfo)!=-1,"stat");

        char *new = malloc((strlen(pathname)+strlen(fajl->d_name)+2)*sizeof(char));
        checkError(new!=NULL,"Malloc");

        strcpy(new, pathname);
        strcat(new, "/");
        strcat(new, fajl->d_name);

        if(S_ISDIR(fileInfo.st_mode)) {
            if(strstr(new,".git") != NULL){
                char *result = realpath(pathname, NULL); //putanja bez gita
                printf("%s\n",result);
                free(result);
            } else{
                howManyGits(new); //nova putanja .../...
            }
        }
        free(new);
    }
    
    checkError(closedir(directory)!=-1,"closedir");
}

int main(int argc, char **argv){

    checkError(argc==2, "args");
    struct stat fileInfo;
    checkError(stat(argv[1],&fileInfo)!=-1,"stat");
    checkError(S_ISDIR(fileInfo.st_mode),"not folder");
    howManyGits(argv[1]);
    exit(EXIT_SUCCESS);
}