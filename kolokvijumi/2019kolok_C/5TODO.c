#define _XOPEN_SOURCE 500
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
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

bool modificationDate(char *pname){
    struct stat fInfo;
    checkError(stat(pname,&fInfo)!=-1,"stat");
    time_t now = time(NULL);
    checkError(now!=-1,"time");
    double diff = (now  - fInfo.st_mtime)/(24*60*60);
    int days = (int)diff + ((diff > (int)diff) ? 1 : 0);
    return days <= 5;
}

void processDir(char *pname, bool prevModDate){
    DIR *dir = opendir(pname);
    checkError(dir!=NULL,"opendir");
    checkError(chdir(pname)!=-1,"chdir");
    struct dirent *entry;
    while((entry=readdir(dir))!=NULL){
        struct stat fInfo;
        //izbegavamo simbolicke linkove
        checkError(lstat(entry->d_name,&fInfo)!=-1,"lstat");
        if(S_ISREG(fInfo.st_mode)){
            if(prevModDate){
                printf("%s\n",entry->d_name);
            }
        }else if(S_ISDIR(fInfo.st_mode)){
            if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0) //ako nismo u roditeljskim folderima
                processDir(entry->d_name,modificationDate(entry->d_name)|prevModDate);
        }
    }
    checkError(closedir(dir)!=-1,"closedir");
    checkError(chdir("..")!=-1,"chdir");
}

int main(int argc,char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"folder");
    processDir(argv[1],modificationDate(argv[1]));
    exit(EXIT_SUCCESS);
}