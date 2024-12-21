#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{\
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0) \

typedef struct{
    int fd;
    char *fName;
}data_t;

int main(int argc, char **argv){

    checkError(argc>1, "args");
    int numFds = argc-1;
    int epollFd = epoll_create(numFds);
    checkError(epollFd!=-1,"epoll_create");
    struct epoll_event *fds = (struct epoll_event*) malloc(numFds*sizeof(struct epoll_event));
    checkError(fds!=NULL,"malloc fds");

    for(int i=0; i<numFds; i++){
        int fd=open(argv[i+1],O_RDONLY | O_NONBLOCK);
        checkError(fd!=-1,"open");

        data_t *data=(data_t*)malloc(sizeof(data_t)); //iskljucivo alokacija
        checkError(data!=NULL,"malloc data");

        data->fd = fd;
        data->fName=argv[i+1];

        fds[i].events = EPOLLIN | EPOLLERR | EPOLLHUP;
        fds[i].data.ptr = data; //pamtimo pokazivac na nasu strukturu
        checkError(epoll_ctl(epollFd,EPOLL_CTL_ADD, fd, &fds[i])!=-1,"epoll_ctl");
    }

    int actNum = numFds;
    struct epoll_event actEv[20];
    float maxsum=0;
    char *resString;
    //int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
    while(actNum){
        int retval=epoll_wait(epollFd,actEv,20,-1);
        //kod polla idu reventi, a zbog epollwaita kod epoll idu eventi
        checkError(retval!=-1,"epoll wait");
        for(int i=0; i<retval; i++){
            if(actEv[i].events & EPOLLIN){
                data_t* data = (data_t*)actEv[i].data.ptr;
                FILE *f = fdopen(data->fd,"r");
                float sum=0,x;
                while(fscanf(f,"%f",&x)!=EOF){
                    sum += x;
                }
                if(maxsum==0 || sum>maxsum){
                    maxsum = sum;
                    resString=data->fName;
                }
            }else if(actEv[i].events & (EPOLLHUP | EPOLLERR)){
                data_t* data = (data_t*)actEv[i].data.ptr;
                checkError(epoll_ctl(epollFd,EPOLL_CTL_DEL,data->fd,&actEv[i])!=-1,"epoll ctl");
                close(data->fd);
                actNum--;
                free(data);
            }
        }
    }
    printf("%s\n",resString);
    free(fds);
    return 0;
}
