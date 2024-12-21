#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){

    checkError(argc==3,"args");
    struct stat fInfo1;
    checkError(stat(argv[1],&fInfo1)!=-1,"stat");
    mode_t usr1 = fInfo.st_mode & (S_IRUSR | S_IWUSR | S_IXUSR);
    mode_t grp1 = fInfo.st_mode & (S_IRGRP | S_IWGRP | S_IXGRP);
    mode_t oth1 = fInfo.st_mode & (S_IROTH | S_IWOTH | S_IXOTH);
    mode_t stariUmask = umask(0);

    mode_t newPermission = (grp << 3) | (oth << 3) | (usr >> 6);
    checkError(chmod(argv[2],newPermission)!=-1,"chmod");

    umask(stariUmask);



    exit(EXIT_SUCCESS);
}
