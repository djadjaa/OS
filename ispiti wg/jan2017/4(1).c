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
    char *fPath=argv[1];
    int a=atoi(argv[2]);
    int b=atoi(argv[3]);
    int fd=open(fPath,O_RDWR);
    checkError(fd!=-1,"open");
    //pozicioniramo se na a
    off_t offset=lseek(fd,a,SEEK_SET);
    checkError(offset!=(off_t)-1,"...");
    struct flock lock;//man fcntl
    lock.l_type = F_WRLCK; //zakljucavamo fajl za pisanje jer on podrazumeva i katanac za pisanje
    lock.l_whence = SEEK_CUR; //zbog offseta je offset daje zaista pocetak fd-a
    lock.l_start = 0;
    lock.l_len = b;
    checkError(fcntl(fd,F_GETLK,&lock)!=-1,"fcntl");
    //proveravamo kao rezultat u polju l_type
    switch(lock.l_type){
        case F_UNLCK:
            printf("unlocked\n");
            break; //mozemo postaviti katanac
        case F_RDLCK:
            printf("shared lock\n");
            break;
        case F_WRLCK:
            printf("exclusive lock\n");
            break;
    }
    close(fd);
    exit(EXIT_SUCCESS);
}
