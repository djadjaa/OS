#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 2
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){

    checkError(argc==2,"");
    int failed=0, success=0;
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"");
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"");

    char c;
    while((c=fgetc(f))>0){
        if(c=='a' || c=='A'){
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_len = 1;
            lock.l_start = ftell(f)-1;
            lock.l_whence = SEEK_SET;

            int res = fcntl(fd,F_SETLK,&lock);

            if(res==-1){
                failed++;
                if (errno != EACCES && errno != EAGAIN) {
				    checkError(0, "lock failed");
			}
            }else{
                //checkError(fcntl(fd, F_SETLK, &lock) != -1, "fcntl failed");
                success++;
                checkError(fseek(f,ftell(f)-1,SEEK_SET)!=-1,"Fseek");
                fprintf(f,"%c",'#');
                
                lock.l_type=F_UNLCK;
                checkError(fcntl(fd,F_SETLK,&lock)!=-1,"fcntl");
            }
        }
    }
    printf("%d %d\n",failed,success);
    fclose(f);
    exit(EXIT_SUCCESS);
}
