#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <libgen.h>
#include <stdbool.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

bool printfiles_f(char *pname){
    char *dirname = basename(pname);
           //izvlacimo prvih 4 slova
    char name_prefix[5] = {dirname[0], dirname[1], dirname[2], dirname[3], '\0'};
    return strcmp(name_prefix,"dir_")==0;
}

void processDir(char *pname, bool printfiles){
    DIR *dir = opendir(pname);
    checkError(dir!=NULL,"opendir");
    checkError(chdir(pname)!=-1,"chdir");
    struct dirent *entry;
    while((entry = readdir(dir))!=NULL){

        struct stat fInfo;
        //izbegavamo simbolicke linkove:
        checkError(lstat(entry->d_name,&fInfo)!=-1,"stat");

        if(S_ISREG(fInfo.st_mode)){
            if(printfiles) //printfiles_p(argv[1]) i rekurzivno nazad
                printf("%s\n",entry->d_name);
        }
        else if(S_ISDIR(fInfo.st_mode)){
            if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0) //ako nismo u roditeljskim folderima
                processDir(entry->d_name,printfiles_f(entry->d_name)||printfiles);
        }
        
    }
    checkError(closedir(dir)!=-1,"closedir");
    checkError(chdir("..")!=-1,"chdir");
}

int main(int argc,char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"not dir");
    processDir(argv[1],printfiles_f(argv[1]));

    exit(EXIT_SUCCESS);
}