#define _XOPEN_SOURCE 500
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>

#define checkError(expr,usrMsg) \
    do{ \
        if (!(expr)){ \
            perror(usrMsg); \
            exit(1); \
        } \
    }while(0)

int main(int argc, char **argv){

    checkError(argc==2,"args");
    struct stat fileInfo;
    checkError(lstat(argv[1],&fileInfo)!=-1,"lstat");

    struct passwd *user=getpwuid(fileInfo.st_uid);
    checkError(user!=NULL,"getpwuid");

    struct group *grp=getgrgid(fileInfo.st_gid);
    checkError(grp!=NULL,"getgrgid");

    printf("%s %s\n",user->pw_name,grp->gr_name);
    exit(EXIT_SUCCESS);
}