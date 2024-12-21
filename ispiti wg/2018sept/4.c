//POLL
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <errno.h>
#include <string.h>
#include <libgen.h> //zbog basename
#include <stdlib.h>

#define checkError(expr,usrMsg) \
    do { \
        if(!(expr)){ \
            perror(usrMsg); \
            exit(EXIT_FAILURE); \
        } \
    }while(0); \

int main(int argc, char **argv){

    checkError(argc>1,"args");
    int nFds = argc-1;
    struct pollfd *fds = malloc(nFds*sizeof(struct pollfd));
    checkError(fds!=NULL,"malloc");
    for(int i=0; i<nFds; i++){
        fds[i].fd = open(argv[i+1],O_RDONLY | O_NONBLOCK);
        checkError(fds[i].fd!=-1,"open");
        fds[i].events = POLLIN | POLLERR | POLLHUP;
        fds[i].revents = 0;
    }
    int actNumFds = nFds;
    int maxNumCh=-1,maxind;
    while(actNumFds>0){
        checkError(poll(fds,nFds,-1)!=-1,"poll");
        for(int i=0; i<nFds; i++){
            if(fds[i].revents & POLLIN){
                char c;
                int count = 0;
                while(read(fds[i].fd,&c,1)>0){
                    if(c=='a'){
                        count++;
                    }
                }
                /*
                if(errno!=EAGAIN)
                    checkError(0,"read");
                */
                if(count > maxNumCh){
                    maxNumCh = count;
                    maxind=i;
                }
            }else if(fds[i].revents  & (POLLERR | POLLHUP)){
                close(fds[i].fd);
                fds[i].fd = -1;
                fds[i].events=0;
                actNumFds--;
            }
            fds[i].revents=0;
        }
    }
    printf("%s %d\n",basename(argv[maxind+1]),maxNumCh);
    free(fds);
    exit(EXIT_SUCCESS);
}
