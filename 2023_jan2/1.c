#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define checkError(expr,usrMsg) \
    do{\
        if(!(expr)){\
            perror(usrMsg); \
            exit(EXIT_FAILURE);\
        }\
    }while(0)

int main(int argc, char **argv){

    checkError(argc==4,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISREG(fInfo.st_mode),"not regular");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");

    int a = atoi(argv[2]);
    int b = atoi(argv[3]);

    char *buff = malloc(fInfo.st_size+1);
    checkError(buff!=NULL,"Malloc");

    checkError(read(fd,buff,fInfo.st_size)!=-1,"read");
    int removed = b-a+1;
    for(int i=a; i<=b; i++)
        printf("%c",buff[i]);
    printf("\n%d\n",removed);

    // Shift content after range to the position of the start of the range
    //The  memmove()  function  copies  n bytes from memory area src to memory area dest.
    memmove(buff + a, buff + b + 1, fInfo.st_size - b);
    
    //The truncate() and ftruncate() functions cause the regular file named by path or referenced by fd to be truncated to a size of precisely length bytes
    checkError(ftruncate(fd,fInfo.st_size-removed)!=-1,"ftruncate");
    checkError(lseek(fd,0,SEEK_SET)!=-1,"lseek");
    checkError(write(fd,buff,fInfo.st_size-removed)!=-1,"write");

    free(buff);
    close(fd);
    exit(EXIT_SUCCESS);
}
