#define _XOPEN_SOURCE 500
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

    checkError(argc==4,"args");
    struct stat fInfo1;
    struct stat fInfo2;
    checkError(stat(argv[1],&fInfo1)!=-1,"stat");
    checkError(stat(argv[2],&fInfo2)!=-1,"stat");
    mode_t stariUmask = umask(0);

    /*
    mode_t usr1 = fInfo1.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR);
    mode_t grp1 = fInfo1.st_mode & (S_IRGRP | S_IWGRP | S_IXGRP);
    mode_t oth1 = fInfo1.st_mode & (S_IROTH | S_IWOTH | S_IXOTH);

    mode_t usr2 = fInfo2.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR);
    mode_t grp2 = fInfo2.st_mode & (S_IRGRP | S_IWGRP | S_IXGRP);
    mode_t oth2 = fInfo2.st_mode & (S_IROTH | S_IWOTH | S_IXOTH);

    mode_t pravaPristupa = (usr1 & usr2) | (grp1 & grp2) | (oth1 & oth2);
    */
    
    mode_t pravaPristupa = fInfo1.st_mode & fInfo2.st_mode;
    checkError(creat(argv[3],pravaPristupa)!=-1,"creat"); //A call to creat() is equivalent to calling open() with flags equal to O_CREAT|O_WRONLY|O_TRUNC
    printf("%d\n",pravaPristupa);
    umask(stariUmask);

    exit(EXIT_SUCCESS);
}