#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <ftw.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

#define MAX 8096

int reg=0,dir=0,kar=0,blok=0,lnk=0,fifo=0,sock=0,uk=0;

int processDir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){

    /*
    u direktorijumu posmatramo apsolutno svaki fajl i proveravamo kog je tipa i na osnovu toga inkr glob promenljive
     */

    if(S_ISREG(sb->st_mode))
        reg++;
    else if(S_ISDIR(sb->st_mode))
        dir++;
    else if(S_ISCHR(sb->st_mode))
        kar++;
    else if(S_ISBLK(sb->st_mode))
        blok++;
    else if(S_ISFIFO(sb->st_mode))
        fifo++;
    else if(S_ISLNK(sb->st_mode))
        lnk++;
    else if(S_ISSOCK(sb->st_mode))
        sock++;
    uk++;

    return 0;
}

int main(int argc,char **argv){

    checkError(argc==2,"args");
    checkError(nftw(argv[1],processDir,50,FTW_PHYS)!=-1,"nftw");
    printf("%d %d %d %d %d %d %d %d\n",reg,dir,kar,blok,lnk,fifo,sock,uk);
    exit(EXIT_SUCCESS);
}
//regularni direktorijumi karakterski blokovski linkovi FIFO soketi ukupno. 