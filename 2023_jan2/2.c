#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#define checkError(expr,usrMsg) \
    do{\
        if(!(expr)){\
            perror(usrMsg); \
            exit(EXIT_FAILURE);\
        }\
    }while(0)

void processDir(char *pname){
    DIR *dir=opendir(pname);
    checkError(dir!=NULL,"opendir");
    checkError(chdir(pname)!=-1,"chdir");
    struct dirent *entry;
    while((entry = readdir(dir))!=NULL){

        struct stat fInfo;
        checkError(stat(entry->d_name,&fInfo)!=-1,"stat");

        if(S_ISLNK(fInfo.st_mode)){
            int now=time(NULL);
            checkError(now!=-1,"time");
            double diff = (now  - fInfo.st_mtime)/(24*60*60);
            if(diff<=7)
                printf("%s\n",entry->d_name);
        }
        else if(S_ISDIR(fInfo.st_mode)){
            if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0) //ako nismo u roditeljskim folderima
                processDir(entry->d_name);
        }
    }
    checkError(chdir("..")!=-1,"chdir");
    checkError(closedir(dir)!=-1,"closedir");
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"folder");
    processDir(argv[1]);

    exit(EXIT_SUCCESS);
}
