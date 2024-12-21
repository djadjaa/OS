#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define checkError(expr,usrMsg) \
    do { \
        if(!(expr)){ \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0); \

#define MAX 257 //zbog terminirajuce nule

int cetvorocifreni(int x){
    int numDig=0;
    while(x!=0){
        numDig++;
        x /= 10;
    }
    if(numDig==4)
        return 1;
    else
        return 0;
}

int isNum(char *s){
    int val=atoi(s);
    if(val>0 && cetvorocifreni(val))
        return 1;
    else
        return 0;
}

int lock(int fd, int start, int len, int mode){
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;
    if(mode==0){ //ako je zakljucano
        if(fcntl(fd,F_SETLK,&lock)==-1){
            return 0;
        }
    }else if(mode==1){ //inace otkljucamo
        lock.l_type = F_UNLCK;
        if(fcntl(fd,F_SETLK,&lock)==-1){
            return 0;
        }
    }
    return 1;
}

int mode(int fd, int start, int len){
    /*
     lseek()  repositions the file offset of the open file description associated with the file descriptor fd to the argument offset according to  the directive whence as follows:
     */
    off_t seekVal = lseek(fd,-5,SEEK_CUR);
    if(seekVal ==(off_t)-1)
        return -1;
    char lockbuff[4]="####";
    int writeVal = write(fd, lockbuff, 4);
    if(writeVal==-1){
        lseek(fd,5,SEEK_CUR);
        return -2;
    }
    lseek(fd,1,SEEK_CUR);
    return 0;
}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    int fd = open(argv[1],O_RDWR);
    checkError(fd!=-1,"open");
    char buff[MAX];
    int i=0;
    int wrdStart = 0;
    int offset=0;
    char c;
    while(read(fd,&c,1)>0){
        if(c==' ' || c=='\n'){
            buff[i] = '\0';
            int res = isNum(buff);
            if(res==1){ //pozitivan četvorocifren broj -> zakljucaj
                int bufflen = strlen(buff);
                if(lock(fd, wrdStart, bufflen, 0)==1){
                    //ako je zakljucano, zameni taj broj sa ####
                    int modeRes = mode(fd, wrdStart, bufflen);

                    checkError(modeRes!=-1,"failed seek");
                    checkError(modeRes!=-2,"failed write");

                    lock(fd,wrdStart,bufflen,1); //otkljucaj
                }/*else{

                }*/
            }/*else{

            }*/
            wrdStart = offset+1;
            i=0;
        }else{
            buff[i] = c;
            i++;
        }
        offset++;
    }
    close(fd);
    exit(EXIT_SUCCESS);
}
