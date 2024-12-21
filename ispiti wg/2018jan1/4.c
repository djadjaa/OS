#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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
    struct pollfd *fds = (struct pollfd*)malloc(numFds*sizeof(struct pollfd));
    checkError(fds!=NULL,"malloc");
    int *arrayCount = malloc(numFds*sizeof(int));
    checkError(arrayCount!=NULL,"malloc");
    for(int i=0; i<numFds; i++){
        int fd = open(argv[i+1],O_RDONLY | O_NONBLOCK);
        checkError(fd!=-1,"open");
        fds[i].fd = fd;
        fds[i].events = POLLIN | POLLERR | POLLHUP;
        fds[i].revents = 0;
    }
    int actFds = numFds;
    int maxCh = 0;
    char *resStr;
    char buffer[4096];
    while(actFds){
        int retVal = poll(fds,numFds,-1);
        checkError(retVal!=-1,"poll");
        for(int i=0; i<numFds; i++){
            //arrayCount[i] = 0;
            if(fds[i].revents & POLLIN){
                int readBytes=0;
                while((readBytes=read(fds[i].fd,buffer,4096))>0){
                    arrayCount[i] += readBytes;
                }
                if(readBytes==-1){
                    if(errno!=EAGAIN) //Resource temporarily unavailable
                        checkError(0,"read");
                }
                if(arrayCount[i]>maxCh){
                    maxCh = arrayCount[i];
                    resStr = argv[i+1];
                }
                fds[i].revents=0;
            }else if(fds[i].revents & (POLLERR | POLLHUP)){
                close(fds[i].fd);
                fds[i].fd =-1;
                fds[i].events=0;
                fds[i].revents=0;
                actFds--;
            }
        }
    }
    printf("%s\n",resStr); //ime sa dete direktorijuma
    free(fds);
    free(arrayCount);
    exit(EXIT_SUCCESS);
}
