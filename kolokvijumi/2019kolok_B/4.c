#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);


int main(int argc,char **argv){

    checkError(argc==3,"args");
    struct stat fInfo1;
    struct stat fInfo2;
    checkError(stat(argv[1],&fInfo1)!=-1,"stat");
    checkError(stat(argv[2],&fInfo2)!=-1,"stat");

    //prebacujemo iz jednog fajla u drugi prava pristupa
    mode_t usr = fInfo1.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR);
    mode_t grp = fInfo1.st_mode & (S_IRGRP | S_IWGRP | S_IXGRP);
    mode_t oth = fInfo1.st_mode & (S_IROTH | S_IWOTH | S_IXOTH);

    mode_t oldUmask = umask(0);

    mode_t pravaPristupaNovo = (grp << 3) | (oth << 3) | (usr >> 6);
    checkError(chmod(argv[2],pravaPristupaNovo)!=-1,"chmod");

    umask(oldUmask);

    exit(EXIT_SUCCESS);
}