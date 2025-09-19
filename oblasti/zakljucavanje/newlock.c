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

    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"");
    struct flock lock;
    lock.l_start = 12;
    lock.l_len = 3;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;

    int ret = fcntl(fd,F_SETLK,&lock);
    printf("%d\n",ret);
    sleep(30);

    exit(EXIT_SUCCESS);
}
