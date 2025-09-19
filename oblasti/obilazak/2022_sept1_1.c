#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <time.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

void dayInWeek(time_t day){
    switch(day){
        case 0:
            printf("nedelja\n");
            break;
        case 1:
            printf("ponedeljak\n");
            break;
        case 2:
            printf("utorak\n");
            break;
        case 3:
            printf("sreda\n");
            break;
        case 4:
            printf("cetvrtak\n");
            break;
        case 5:
            printf("petak\n");
            break;
        case 6:
            printf("subota\n");
            break;
    }
}

int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    if(S_ISREG(sb->st_mode)){
        printf("%s ",fpath+ftwbuf->base);
        //posmatramo vreme modifikacije:
        struct tm *time = localtime(&(sb->st_mtime));
        checkError(time!=NULL,"local time");
        dayInWeek(time->tm_wday);
    }
    
    return 0;
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"");
    checkError(nftw(argv[1],processDir,50,0)!=-1,"");

    exit(EXIT_SUCCESS);
}
