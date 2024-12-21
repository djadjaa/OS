#define _POSIX_C_SOURCE 2
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

#define MAX 256

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");

    char buff[MAX];

    while(fscanf(f,"%s",buff)!=EOF){
        int num = atoi(buff);
        if(num>=1000 && num<=9999){
            struct flock lock;
            //ftell() obtains the current value of the file position indicator for the stream pointed to by stream.
            int poz = ftell(f);

            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = poz - 4;
            lock.l_len = 4;

            if(fcntl(fd,F_SETLK,&lock)!=-1){
                //fseek() sets the file position indicator for the stream pointed to by stream.
                checkError(fseek(f,poz-4,SEEK_SET)!=-1,"Fseek");
                fprintf(f,"%s","####");
            }
        }
    }

    fclose(f);
    exit(EXIT_SUCCESS);
}
