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
    struct flock lock;
    lock.l_type = F_WRLCK; //zakljucavamo fajl za pisanje
    lock.l_whence = SEEK_SET;
    lock.l_start = a;
    lock.l_len = b;
    checkError(fcntl(fd,F_GETLK,&lock)!=-1,"fcntl");

    /*
     U slučaju da sekcija fajla [a, a+b] nije zaključana ispisati na standardni izlaz unlocked, u slučaju da je zaključana da može da se čita, a ne može da se piše ispisati shared lock i na kraju ako je zaključana i za čitanje i za pisanje ispisati exclusive lock.
     */

    if(lock.l_type==F_UNLCK){//ako segment nije zakljucan
        printf("unlocked\n");
    }else if(lock.l_type==F_WRLCK){
        printf("exclusive lock\n");
    }else if(lock.l_type==F_RDLCK){
        printf("shared lock\n");
    }

    close(fd);
    exit(EXIT_SUCCESS);
}
