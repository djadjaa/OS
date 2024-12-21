#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

char *reverse(char *buff, int size){
    char *res = (char*)malloc(size);
    checkError(res!=NULL,"malloc");

    int i = size-1, j=0;
    while(j<size)
        res[j++] = buff[i--];

    return res;
}
int main(int argc,char **argv){

    checkError(argc==4,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISREG(fInfo.st_mode),"not regular");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");
    int offset = atoi(argv[2]);
    int size = atoi(argv[3]);
    checkError(lseek(fd,offset,SEEK_SET)!=-1,"lseek");
    char *buff = malloc(size+1);
    checkError(buff!=NULL,"malloc");
    checkError(read(fd,buff,size)!=-1,"read");
    char *reversestr = reverse(buff,size);
    checkError(lseek(fd, offset, SEEK_SET) != -1, "lseek"); //citamo onoliko koliko treba da zamenimo u tekstu 
    checkError(write(fd,reversestr,size)!=-1,"write");

    free(reversestr);
    free(buff);
    close(fd);

    exit(EXIT_SUCCESS);
}