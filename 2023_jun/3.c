#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \


int main(int argc, char **argv){

    checkError(argc==4,"args");
    int a = atoi(argv[2]);
    int b = atoi(argv[3]);
    int fd = open(argv[1],O_RDONLY);
    checkError(fd!=-1,"open");

    checkError(lseek(fd,a,SEEK_SET)!=-1,"lseek");

    char *buff = malloc(b+1);
    checkError(buff!=NULL,"malloc");

    checkError(read(fd,buff,b)!=-1,"read");

    //printf("%s\n",buff);
    mode_t oldUmask = umask(0);
    mode_t pravaPristupa = 0777;
    checkError(mkdir(buff,pravaPristupa)!=-1,"mkdir");
    umask(oldUmask);

    free(buff);
    close(fd);
    exit(EXIT_SUCCESS);
}