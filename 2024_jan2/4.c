#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <limits.h>
#include <sys/wait.h>
//#define MAX (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define MAX 8096

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

void process_event(struct inotify_event *i, char* path){
    
    if(!(i -> mask & IN_MOVED_TO)) return;
    
    pid_t child = fork();
    if(child == 0){
        chdir(path);
        execl("/bin/cat", "cat", i->name, NULL);
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
        return;
    }
    
    /*
    //citamo iz fajlova koji su premesteni u direktorijum
    int fd = open(i->name,O_RDONLY);
    //printf("%s\n",event->name);
    checkError(fd!=-1,"open");
    char buff[MAX];
    int readBytes=0;
    while((readBytes=read(fd,buff,MAX))>0){
        checkError(readBytes!=-1,"read");
        checkError(write(STDOUT_FILENO,buff,readBytes)!=-1,"write");
    }
    close(fd);
    

    */

    
    return;
}

void processEvents(char *buf, ssize_t numRead, char *dirpath){
    char *p=NULL;
    for(p = buf; p<buf+numRead;){
        struct inotify_event *event = (struct inotify_event*)p;

        process_event(event, dirpath);

        p+=sizeof(struct inotify_event)+event->len;
    }

}

int main(int argc, char **argv){

    checkError(argc==2,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    checkError(S_ISDIR(fInfo.st_mode),"not folder");
    int inotifyFd = inotify_init();
    checkError(inotifyFd!=-1,"inotify init");
    int wd = inotify_add_watch(inotifyFd,argv[1],IN_MOVED_TO);
    checkError(wd!=-1,"inotify add watch");

    printf("Watching %s using wd %d\n", argv[1], wd);

    char buff[MAX];
    while (1)
    {
        size_t numRead  = read(inotifyFd,buff,MAX);
        checkError(numRead!=-1,"read from inotify fd");
        if(numRead==0) 
            printf("read() from inotify fd returned 0!");
        processEvents(buff,numRead,argv[1]);
    }
    
    
    exit(EXIT_SUCCESS);
}
