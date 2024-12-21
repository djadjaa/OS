#define _XOPEN_SOURCE 700
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

#define MAX 256

int main(int argc, char **argv){

    checkError(argc==2,"");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"");
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"");
    char buff[MAX];
    while(fscanf(f,"%s",buff)!=EOF){
        if(strncmp(buff,"_test",5)==0){
            struct flock lock;
            int len = strlen(buff);
            lock.l_start = ftell(f)-len;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_len = len;

            int res = fcntl(fd,F_SETLK,&lock);
            if(res==-1){
                //checkError(0,"");
                if(errno != EACCES && errno != EAGAIN)
                    checkError(0, "puklo");
            }else{
                checkError(fseek(f,ftell(f)-len,SEEK_CUR)!=-1,"");
                for(int i=0; buff[i]!='\0'; i++)
                    buff[i] ='#';
                fprintf(f,"%s",buff);
                
                /*
                
                lock.l_type =F_UNLCK;
                checkError(fcntl(fd,F_SETLK,&lock)!=-1,"");
                 */
            }
        }
    }
    fclose(f);
    exit(EXIT_SUCCESS);
}
