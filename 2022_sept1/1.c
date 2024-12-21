#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <ftw.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

void dayInWeek(time_t day){
    switch (day){
        case 1:
            printf("ponedeljak ");
            break;
        case 2:
            printf("utorak ");
            break;
        case 3:
            printf("sreda ");
            break;
        case 4:
            printf("cetvrtak ");
            break;
        case 5:
            printf("petak ");
            break;
        case 6:
            printf("subota ");
            break;
        case 0:
            printf("nedelja ");
            break;
    }
}

int processDir(const char *fpath, const struct stat *sb,int typeflag, struct FTW *ftwbuf){

    if(S_ISREG(sb->st_mode)){
        struct tm *time = localtime(&(sb->st_mtime));
        checkError(time!=NULL,"localtime");
        printf("%s ",fpath+ftwbuf->base);
        dayInWeek(time->tm_wday);
        putchar('\n');
    }


    return 0;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"not folder");
    checkError(nftw(argv[1],processDir,50,0)!=-1,"nftw");

    exit(EXIT_SUCCESS);
}