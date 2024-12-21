#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 2
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define MAX 256

int velikaSlovaEkspl(char *s){
    for(int i=0; s[i]!='\0'; i++){
        if(islower(s[i]) || isdigit(s[i]) || !(isalpha(s[i]))){
            return 1;
        } 
    }
    return 0;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");
    FILE *f = fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");
    
    char buff[MAX];
    int numChanged=0;
    while(fscanf(f,"%s",buff)==1){
        if(velikaSlovaEkspl(buff)==0){
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = ftell(f);
            lock.l_len = -strlen(buff);

            int res = fcntl(fd,F_SETLK,&lock);
            if(res==-1){
                checkError(0,"fcntl");
            }
                
            else{
                checkError(fseek(f,-strlen(buff),SEEK_CUR)!=-1,"FSEEK");
                for(int i=0; buff[i]!='\0'; i++)
                    buff[i] = '#';
                fprintf(f,"%s",buff);
                numChanged+=strlen(buff);

                lock.l_type = F_UNLCK;
                checkError(fcntl(fd,F_SETLK,&lock)!=-1,"fcntl");
            }
        }
    }
    printf("%d\n",numChanged);
    fclose(f);

    exit(EXIT_SUCCESS);
}
