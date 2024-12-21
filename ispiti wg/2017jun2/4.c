#define _XOPEN_SOURCE 700

#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <stdbool.h>
#include <string.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){
    checkError(argc>1,"args");
    int nFds=argc-1;
    struct pollfd *fds=malloc(nFds*sizeof(struct pollfd));
    checkError(fds!=NULL,"malloc");
    FILE **files=malloc(nFds*sizeof(*files));
    checkError(files!=NULL,"malloc");
    for(int i=0; i<nFds; i++){
        fds[i].fd=open(argv[i+1],O_RDONLY | O_NONBLOCK);
        checkError(fds[i].fd!=-1,"open");
        files[i] = fdopen(fds[i].fd,"r");
        checkError(files[i]!=NULL,"fdopen");
        fds[i].events=POLLIN;
    }
    int numAct=nFds;
    char *rez=NULL;
    int max=0;
    bool flag=false; //inicijalizacija
    while(numAct>0){
        int retVal=poll(fds,nFds,-1);
        checkError(retVal!=-1,"poll");
        for(int i=0; i<nFds; i++){
            if(fds[i].revents & POLLIN){
                int number;
                while(fscanf(files[i],"%d",&number)==1){
                    if(!flag){
                        max=number;
                        rez=argv[i+1];
                        flag=true;
                    }else{
                        if(number>max){
                            max=number;
                            rez=argv[i+1];
                        }
                    }
                    fds[i].revents=0;
                }
            }else if(fds[i].revents & (POLLERR | POLLHUP)){
                close(fds[i].fd);
                fds[i].events=0;
                fds[i].fd=-1;
                fds[i].revents = 0;
                numAct--;
            }
        }
    }
    char *tmp=strrchr(rez,'/');
    printf("%d %s\n",max,tmp!=NULL ? tmp+1 : rez);
    free(fds);
    free(files);
    exit(EXIT_SUCCESS);
}
