#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#define checkError(expr,usrMsg)\
    do{\
        if(!(expr)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0);

void upperToLowerCaseVV(char *s,int n){

    for(int i=0; i<n; i++){
        if(isupper(s[i])){
            s[i] = tolower(s[i]);
        }
        else if(islower(s[i])){
            s[i] = toupper(s[i]);
        }
    }
}

int main(int argc,char **argv){
    checkError(argc==4,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");
    int offset=atoi(argv[2]);
    int size=atoi(argv[3]);
    checkError(lseek(fd,offset,SEEK_SET)!=-1,"LSEEK");
    char *buff = malloc(size+1);
    checkError(buff!=NULL,"buff");
    checkError(read(fd,buff,size)!=-1,"read");
    //buff[size]='\0';
    
    upperToLowerCaseVV(buff,size);
    checkError(lseek(fd,offset,SEEK_SET)!=-1,"lseek");
    checkError(write(fd,buff,size)!=-1,"write");
    free(buff);
    close(fd);
    exit(EXIT_SUCCESS);
}