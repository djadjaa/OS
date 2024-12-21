#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

time_t now;

void processDir(char *path){
    struct stat fInfo;
        //izbegavamo simbolicke linkove:
    checkError(lstat(path,&fInfo)!=-1,"stat");

    if(S_ISREG(fInfo.st_mode)){
        int numdays = ceil((double)(now-fInfo.st_mtime)/(24*60*60));
        if(numdays<=5){
            char *name = strrchr(path,'/');
            printf("%s\n",name+1);
        }
    }
    else if(S_ISDIR(fInfo.st_mode)){
        DIR *dir = opendir(path);
        checkError(dir!=NULL,"opendir");
        //checkError(chdir(path)!=-1,"chdir");
        struct dirent *entry=NULL;
        errno=0;
        while((entry = readdir(dir))!=NULL){

            if(strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0)
                continue;
            char *tmp = malloc((strlen(path)+strlen(entry->d_name)+2)*sizeof(char));
            checkError(tmp!=NULL,"");
            strcpy(tmp,path);
            strcat(tmp,"/");
            strcat(tmp,entry->d_name);

            processDir(tmp);
            free(tmp);
        }
        checkError(errno!=EBADF,"");
        errno=0;
        checkError(closedir(dir)!=-1,"closedir");
        //checkError(chdir("..")!=-1,"chdir");

    }
    
    
}


int main(int argc,char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(lstat(argv[1],&fInfo)!=-1,"");
    checkError(S_ISDIR(fInfo.st_mode),"");
    checkError(time(&now)!=-1,"");
    processDir(argv[1]);

    exit(EXIT_SUCCESS);
}