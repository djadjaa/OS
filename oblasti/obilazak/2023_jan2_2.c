#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <ftw.h>
#include <stdlib.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 256

time_t now = 0;
char res[MAX];

int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    if(S_ISLNK(sb->st_mode)){
        if(sb->st_mtime > now){
            now = sb->st_mtime;
            strcpy(res,fpath+ftwbuf->base);
        }
    }
    return 0;
}

int main(int argc, char **argv){

    checkError(argc==2,"");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"");
    checkError(S_ISDIR(fInfo.st_mode),"");
    //FTW_PHYS koristimo ovaj makro posto radimo nftw nad simbolickim linkovima
    checkError(nftw(argv[1],processDir,50,FTW_PHYS)!=-1,"");

    printf("%s\n",res);

    exit(EXIT_SUCCESS);
}
