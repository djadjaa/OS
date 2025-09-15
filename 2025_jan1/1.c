#define _XOPEN_SOURCE 700

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>

#define checkError(expr, usrMsg) \
    do{ \
        if(!(expr)){ \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0); \

int main(int argc, char **argv){

    checkError(argc==2, "");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"");
    if(S_ISREG(fInfo.st_mode)){
        printf("r");
    }
    else if(S_ISDIR(fInfo.st_mode)){
        printf("d");
    }
    else if(S_ISFIFO(fInfo.st_mode)){
        printf("p");
    }
    else if(S_ISLNK(fInfo.st_mode)){
        printf("l");
    }
    else if(S_ISSOCK(fInfo.st_mode)){
        printf("s");
    }
    else if(S_ISCHR(fInfo.st_mode)){
        printf("c");
    }
    else if(S_ISBLK(fInfo.st_mode)){
        printf("b");
    }
    printf(" %ld",fInfo.st_size);
    struct passwd *userInfo = getpwuid(fInfo.st_uid);
    checkError(userInfo!=NULL,"");
    printf(" %s\n",userInfo->pw_name);


    exit(EXIT_SUCCESS);
}