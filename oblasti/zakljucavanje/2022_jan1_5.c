#define _POSIX_C_SOURCE 2
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 255
int main(int argc, char **argv){

    checkError(argc==2,"args");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"");
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"");
    int numCharChanged = 0;
    char buff[MAX];
    while(fscanf(f,"%s",buff)==1){
        //err ce da nam pokaze na prvi karakter koji nije broj
        char *err=NULL;
        int num = strtol(buff,&err,10);
        if(*buff!='\0' && *err=='\0'){
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_start = ftell(f);
            lock.l_len = -strlen(buff);
            lock.l_whence = SEEK_SET;

            int retval = fcntl(fd,F_SETLK,&lock);
            if(retval!=-1){
                for(int i=0; buff[i]!='\0'; i++)
                    buff[i] = '#';
                fseek(f,-strlen(buff),SEEK_CUR);
                fprintf(f,"%s",buff);
                numCharChanged+=strlen(buff);

                lock.l_type = F_UNLCK;
                checkError(fcntl(fd,F_SETLK,&lock)!=-1,"");
            }else{
                if(errno!=EACCES && errno!=EAGAIN)   
                    checkError(0,"errno");
                continue;
            }
        }
    }
    printf("%d\n",numCharChanged);
    fclose(f);
    exit(EXIT_SUCCESS);
}
