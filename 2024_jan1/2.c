#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>

#define checkError(expr,usrMsg) \
    do{ \
        if (!(expr)){ \
            perror(usrMsg); \
            exit(1); \
        } \
    }while(0)

void fifos(char *pathname){
    DIR *directory = opendir(pathname);
    checkError(directory!=NULL,"opendir");

    struct dirent *files;
    while((files = readdir(directory))!=NULL){
        if(strcmp(files->d_name,".")==0 || strcmp(files->d_name,"..")==0)
            continue;
        struct stat fileInfo;
        checkError(stat(pathname,&fileInfo)!=-1,"stat");
        char *new = malloc((strlen(pathname)+strlen(files->d_name)+2)*sizeof(char)); //alociranje memorije za novu putanju
        checkError(new!=NULL,"malloc");
        
        //formiranje nove putanje
        strcpy(new,pathname);
        strcat(new,"/");
        strcat(new,files->d_name);

        if(S_ISFIFO(fileInfo.st_mode)){
            if((fileInfo.st_mode & S_IRGRP) && (fileInfo.st_mode & S_IWGRP)){
                struct passwd *usr = getpwuid(fileInfo.st_uid);
                printf("%s: %s\n",new,usr->pw_name);
                free(new);
            }else{
                fifos(new);
            }
        }

        free(new);
    }

    checkError(closedir(directory)!=-1,"closedir");
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    struct stat fileInfo;
    checkError(stat(argv[1],&fileInfo)!=-1,"stat");
    checkError(!S_ISDIR(fileInfo.st_mode),"folder");
    fifos(argv[1]);

    exit(EXIT_SUCCESS);
}