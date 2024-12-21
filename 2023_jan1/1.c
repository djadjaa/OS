#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pwd.h>
#include <libgen.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);


int main(int argc,char **argv){

    checkError(argc>=2,"args");
    struct stat fInfo[argc];
    int maxsize=0;
    char *resstr1 = NULL;
    char *resstr2 = NULL;
    for(int i=1; i<argc; i++){
        checkError(stat(argv[i],&fInfo[i])!=-1,"stat");
        if(fInfo[i].st_size > maxsize) {
            maxsize = fInfo[i].st_size;
            struct passwd *usr = getpwuid(fInfo[i].st_uid);
            resstr1 = usr->pw_name;
            resstr2 = basename(argv[i]);
        }
    }
    printf("%s %d %s\n",resstr1,maxsize,resstr2);
    exit(EXIT_SUCCESS);
}