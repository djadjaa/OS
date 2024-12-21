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

#define MAX_LEN
char *reverse(char *s, int len){
    char *newStr = malloc((len+1)*sizeof(char));
    checkError(newStr!=NULL,"malloc");
    int i=len-1, j=0;
    while(i>=0){
        newStr[j++] = s[i--];
    }
    newStr[j]='\0';
    return newStr;
}

int otkljucano(int fd, int start, int len){
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;

    checkError(fcntl(fd,F_GETLK,&lock)!=-1,"fcntl");

    if(lock.l_type == F_UNLCK){
        return 1;
    }else{
        return 0;
    }
}

void mode(int fd, int start, int len, char *buffer){
    lseek(fd,start,SEEK_SET); //lseek menja poziciju fd-a
    //idemo od pocetka fajla do pocetka reci koja je otkljucana
    checkError(write(fd,buffer,len)!=-1,"write"); //pisemo od pocetka reci do njenog kraja
    lseek(fd,1,SEEK_CUR); //ostajemo na poziciji kraja reci
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");
    char buff[MAX_LEN+1];
    int brIzmenjenih=0, brZakljucanih=0;
    int i=0, offset=0, wrdStart=0, buffLen=0;
    char c;
    while(read(fd,&c,1)>0){
        if(c==' ' || c=='\n'){
            buff[i] = '\0';

            if(otkljucano(fd,wrdStart,buffLen)==1) { //ako je otkljucano, zamenjujemo otkljucane reci sa onim obrnutim
                char *rvrs = reverse(buff,buffLen);
                mode(fd, wrdStart, buffLen, rvrs);
                free(rvrs);
                brIzmenjenih++;
            }else{
                brZakljucanih++;
            }

            wrdStart = offset+1;
            buffLen=0;
            i=0;
        }
        else{
            buff[i] = c;
            i++;
            buffLen++;
        }
        offset++;
    }
    printf("%d %d\n", brIzmenjenih, brZakljucanih);
    close(fd);
    exit(EXIT_SUCCESS);
}
