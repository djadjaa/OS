#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/epoll.h>
#include <errno.h>

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

typedef struct{
    int fd;
    char *fPath;
}data_t;

int main(int argc, char **argv){

    checkError(argc>1, "args");
    int numFds=argc-1;

    int epollFd = epoll_create(numFds);
    checkError(epollFd!=-1,"epoll_create");

    struct epoll_event* fds = (struct epoll_event*) malloc(numFds*sizeof(struct epoll_event));
    checkError(fds!=NULL,"malloc");

    int *numChars = malloc(numFds*sizeof(int));
    checkError(numChars!=NULL,"malloc");
    for(int i=0; i<numFds; i++){
        int fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
        checkError(fd!=-1,"open");
        numChars[i] = 0;

        data_t* data = (data_t*)malloc(sizeof(data_t));
        data->fd = fd;
        data->fPath = argv[i+1];

        fds[i].events = EPOLLIN | EPOLLERR | EPOLLHUP;
        fds[i].data.ptr = data;

        checkError(epoll_ctl(epollFd,EPOLL_CTL_ADD,fd,&fds[i])!=-1,"epoll_ctl");
    }
    int actFds = numFds;
    int minCh = INT_MAX;
    char *strRes;
    char buffer[4096];

    struct epoll_event actEv[20];
    while(actFds){
        int retVal = epoll_wait(epollFd,actEv,20,-1);
        checkError(retVal!=-1,"poll");
        for(int i=0; i<retVal; i++){
            if(actEv[i].events & EPOLLIN){
                data_t* data = (data_t*)actEv[i].data.ptr;
                int fd = data->fd;
                int readBytes=0;
                while((readBytes=read(fd,buffer,4096))>0){
                    numChars[i] += readBytes;
                }
                if(readBytes==-1){
                    if(errno != EAGAIN) //zbog O_NONBLOCK
                        checkError(0,"read");
                }
                if(numChars[i] < minCh){
                    minCh = numChars[i];
                    strRes = argv[i+1];
                }
            }else if(actEv[i].events & (EPOLLERR | EPOLLHUP)){
                data_t* data = (data_t*)actEv[i].data.ptr;
                checkError(epoll_ctl(epollFd,EPOLL_CTL_DEL,data->fd,&actEv[i])!=-1,"epoll ctl");
                close(data->fd);
                actFds--;
                free(data);
            }
        }
    }
    printf("%s\n",strRes);
    free(numChars);
    free(fds);
    exit(EXIT_SUCCESS);
}
