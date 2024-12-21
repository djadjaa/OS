#define _DEFAULT_SOURCE

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>

#define checkError(expr,userMsg) \
    do { \
        if(!(expr)){ \
            perror(userMsg); \
            exit(1); \
        } \
    }while(0)

int main(int argc, char **argv){

    checkError(argc>=2,"args");
    struct stat fileInfo;
    for(int i=1; i<argc; i++){
        checkError(stat(argv[i],&fileInfo)!=-1,"stat");
        struct passwd *korisnik = getpwuid(fileInfo.st_uid);
        checkError(korisnik!=NULL,"getpwuid");
        printf("%s:", korisnik->pw_name);

        int ngroups = 100; //koliko maks grupa mozemo imati
        gid_t *groups = malloc(sizeof(*groups) * ngroups);

        checkError(groups!=NULL,"malloc");
        checkError(getgrouplist(korisnik->pw_name, korisnik->pw_gid, groups, &ngroups) != -1,"getgrouplist");

        for(int i=0; i<ngroups; i++){
            struct group *grupa = getgrgid(groups[i]);
            printf(" %s", grupa->gr_name);
        }
        printf("\n");
        free(groups);
    }

    exit(EXIT_SUCCESS);
}