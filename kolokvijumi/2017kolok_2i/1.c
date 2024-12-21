#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define checkError(expr,usrMsg) \
    do{\
        if(!(expr)){\
            perror(usrMsg); \
            exit(EXIT_FAILURE);\
        }\
    }while(0)

bool osIsPublicFile(const char *fpath){
    struct stat fInfo;
    checkError(stat(fpath,&fInfo)!=-1,"stat");
    checkError(S_ISREG(fInfo.st_mode),"not regular");
    if((fInfo.st_mode & S_IROTH) && (fInfo.st_mode & S_IWOTH))
        return true;
    else
        return false;
}

int main(int argc, char **argv){
    checkError(argc==2,"args");
    bool res = osIsPublicFile(argv[1]);
    printf("%s\n", (res==true) ? "true" : "false");

    exit(EXIT_SUCCESS);
}