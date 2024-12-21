#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

void osMkPublicDir(const char *dname){
    mode_t pravaPristupa = 0777;
    mode_t oldumask = umask(0);
    checkError(mkdir(dname,pravaPristupa)!=-1,"mkdir");
    umask(oldumask);
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    osMkPublicDir(argv[1]);

    exit(EXIT_SUCCESS);
}