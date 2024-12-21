#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

void writeChmod(char *fpath){
    char pp[]="xrwxrwxrwx";
    struct stat fInfo;
    checkError(stat(fpath,&fInfo)!=-1,"stat");
    switch(fInfo.st_mode & S_IFMT){
        case S_IFSOCK:
            pp[0] = 's';
            break;
        case S_IFLNK:
            pp[0] = 'l';
            break;
        case S_IFREG:
            pp[0] = '-';
            break;
        case S_IFBLK:
            pp[0] = 'b';
            break;
        case S_IFDIR:
            pp[0] = 'd';
            break;
        case S_IFCHR:
            pp[0] = 'c';
            break;
        case S_IFIFO:
            pp[0] = 'f';
            break;
    }

    if(!(fInfo.st_mode & S_IRUSR))
        pp[1]='-';
    if(!(fInfo.st_mode & S_IWUSR))
        pp[2] = '-';
    if(!(fInfo.st_mode & S_IXUSR))
        pp[3] = '-';
    if(!(fInfo.st_mode & S_IRGRP))
        pp[4] = '-';
    if(!(fInfo.st_mode & S_IWGRP))
        pp[5] = '-';
    if(!(fInfo.st_mode & S_IXGRP))
        pp[6] = '-';
    if(!(fInfo.st_mode & S_IROTH))
        pp[7] = '-';
    if(!(fInfo.st_mode & S_IWOTH))
        pp[8] = '-';
    if(!(fInfo.st_mode & S_IXOTH))
        pp[9]='-';

    printf("%s\n",pp);
}

int main(int argc,char **argv){

    checkError(argc==2,"args");
    writeChmod(argv[1]);

    exit(EXIT_SUCCESS);
}
/*
S_IFSOCK   0140000   socket
           S_IFLNK    0120000   symbolic link
           S_IFREG    0100000   regular file
           S_IFBLK    0060000   block device
           S_IFDIR    0040000   directory
           S_IFCHR    0020000   character device
           S_IFIFO    0010000   FIFO


S_IRWXU     00700   owner has read, write, and execute permission
           S_IRUSR     00400   owner has read permission
           S_IWUSR     00200   owner has write permission
           S_IXUSR     00100   owner has execute permission

           S_IRWXG     00070   group has read, write, and execute permission
           S_IRGRP     00040   group has read permission
           S_IWGRP     00020   group has write permission
           S_IXGRP     00010   group has execute permission

           S_IRWXO     00007   others (not in group) have read, write, and exe‐
                               cute permission
           S_IROTH     00004   others have read permission
           S_IWOTH     00002   others have write permission
           S_IXOTH     00001   others have execute permission

*/