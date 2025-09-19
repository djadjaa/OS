#define _DEFAULT_SOURCE
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

    checkError(argc>=2,"");
    struct stat fInfo;
    for(int i=1; i<argc; i++){
        int res = stat(argv[i],&fInfo);
        if(res==-1){
            printf("%s: nepostojeci fajl\n",argv[i]);
        }
        struct passwd *user = getpwuid(fInfo.st_uid);
        checkError(user!=NULL,"");
        printf("%s: ", user->pw_name);
        int ngroups = 100;
        gid_t *groups = malloc(ngroups*sizeof(*groups));
        checkError(groups!=NULL,"");
        checkError(getgrouplist(user->pw_name, user->pw_gid, groups, &ngroups)!=-1,"");

        for(int i=0; i<ngroups; i++){
            struct group *grp = getgrgid(groups[i]);
            printf(" %s",grp->gr_name);
        }
        putchar('\n');
        free(groups);
    }

    exit(EXIT_SUCCESS);
}
