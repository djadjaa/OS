#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 8096

int main(int argc, char **argv){

    checkError(argc==3,"args");
    //jedan fajl otvaramo preko fopen jer iz njega izvlacimo podatke, a drugi preko open-a jer smestamo u njega sadrzaj
    FILE *src = fopen(argv[1],"r");
    checkError(src!=NULL,"fopen");

    int fd = open(argv[2],O_WRONLY);
    checkError(fd!=-1,"open");

    char buff[MAX];
    int offset;

    while(fscanf(src,"%d%s",&offset,buff)==2){
        checkError(lseek(fd,offset,SEEK_SET)!=-1,"lseek");
        checkError(write(fd,buff,strlen(buff))!=-1,"write");
    }
    fclose(src);
    close(fd);

    exit(EXIT_SUCCESS);
}