#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){
    checkError(argc>1,"args");
    int numFds = argc-1;
    struct pollfd *fds = malloc(numFds*sizeof(struct pollfd));
    checkError(fds!=NULL,"malloc");
    for(int i=0; i<numFds; i++){
        fds[i].fd = open(argv[i+1],O_RDONLY | O_NONBLOCK);
        checkError(fds[i].fd!=-1,"open");
        fds[i].events = POLLIN | POLLERR | POLLHUP;
        fds[i].revents = 0;
    }
    int actFds = numFds;
    char buff[1024];
    int maxCh = 0;
    char *resStr;
    while(actFds){
        int retVal = poll(fds,numFds,-1);
        checkError(retVal!=-1,"poll");
        for(int i=0; i<numFds; i++){
            if(fds[i].revents & POLLIN){
                int readBytes = read(fds[i].fd,buff,1023);
                checkError(readBytes!=-1,"read");
                buff[readBytes]=0; //term nula
                if(readBytes>maxCh){
                    maxCh = readBytes;
                    resStr = argv[i+1];
                }
                fds[i].revents=0;
            }else if(fds[i].revents & (POLLERR | POLLHUP)){
                close(fds[i].fd);
                fds[i].fd =-1;
                fds[i].revents=0;
                actFds--;
            }
        }
    }
    printf("%s\n",resStr);
    free(fds);
    exit(EXIT_SUCCESS);
}
