#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);


int main(int argc,char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    char *usr;
    struct passwd *pwd = getpwuid(fInfo.st_uid);
    checkError(pwd!=NULL,"getpwuid");
    usr = malloc(strlen(pwd->pw_name)+1);
    strcpy(usr,pwd->pw_name);

    char* grpname;
	struct group *grp = getgrgid(fInfo.st_gid);
	checkError(grp != NULL, "getgrgid");
	grpname = malloc(strlen(grp->gr_name) + 1);
	strcpy(grpname, grp->gr_name);

    printf("%s %s\n",usr,grpname);

    free(usr);
    free(grpname);
    exit(EXIT_SUCCESS);
}