#define _POSIX_C_SOURCE 2
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define K 20
char *reverse(char *s){
    int n=strlen(s);
    for(int i=0; i<n/2; i++){
        char tmp = s[i];
        s[i]=s[n-i+1];
        s[n-i+1]=tmp;
    }
    return s;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int fd=open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");
    FILE *f=fdopen(fd,"r+");
    checkError(f!=NULL,"fdopen");

    int brIzmenjenih=0,brZakljucanih=0;

    int wrdpt=0,wrdstart=0,wrdlen=0;

    char word[4096];
    char c;

    while(read(fd,&c,1)>0){
        if(c==' ' || c=='\n'){
            word[wrdpt]='\0';
            struct flock lock;
            lock.l_type = F_WRLCK; //za čitanje
            lock.l_whence = SEEK_SET;
            lock.l_start = start;
            lock.l_len = wrdlen;
            checkError(fcntl(fd,F_GETLK,&lock)!=-1,"fcntl");
            if(lock.l_type == F_UNLCK){
                char *reverseStr = reverse(word);
                lseek(fd,start,SEEK_SET);
            }
        }
    }
    exit(EXIT_SUCCESS);
}
