#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){

    checkError(argc==2,"");
    struct stat fInfo;
    checkError(lstat(argv[1],&fInfo)!=-1,"");
    struct passwd *usr = getpwuid(fInfo.st_uid);
    struct group *grp = getgrgid(fInfo.st_gid);
    printf("%s %s\n",usr->pw_name, grp->gr_name);


    exit(EXIT_SUCCESS);
}
