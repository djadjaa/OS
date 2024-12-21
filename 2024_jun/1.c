#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){
    checkError(argc==4,"");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"");
    int a = atoi(argv[2]);
    int b = atoi(argv[3]);
    checkError(S_ISREG(fInfo.st_mode),"Not reg");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");

    char *buff = malloc(fInfo.st_size+1);
    checkError(buff!=NULL,"");

    checkError(read(fd,buff,fInfo.st_size)!=-1,"read");
    int removed = b-a+1;
    for(int i=a; i<=b; i++){
        printf("%c",buff[i]);
    }
    printf("\n%d\n",removed);
    memmove(buff+a,buff+b+1,fInfo.st_size-b);
    checkError(ftruncate(fd,fInfo.st_size-removed)!=-1,"ftruncate");
    checkError(lseek(fd,0,SEEK_SET)!=-1,"LSEEK");
    checkError(write(fd,buff,fInfo.st_size-removed)!=-1,"");

    free(buff);
    close(fd);
    exit(EXIT_SUCCESS);
}
