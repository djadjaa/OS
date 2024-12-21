#define _XOPEN_SOURCE 700

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <ftw.h>

#define checkError(expr,usrMsg) \
    do{\
        if(!(expr)){\
            perror(usrMsg); \
            exit(EXIT_FAILURE);\
        }\
    }while(0)


#define SIZE 8096

static char *dirPath = NULL;

unsigned osNumOfDaysFileModified(const char* fpath){
    struct stat fInfo;
    checkError(stat(fpath,&fInfo)!=-1,"stat");
    time_t now = time(NULL);
    checkError(now!=-1,"time");
    unsigned res = (now - fInfo.st_mtime)/(24*60*60);
    return res;
}

void osMoveFile(const char* srcPath, const char* destPath){
    char *src = realpath(srcPath,NULL);
    char *dest = realpath(destPath,NULL);
    checkError(src!=NULL,"src doesnt exist");
    if(src!=NULL && dest!=NULL){
        checkError(strcmp(src,dest)!=0,"equal");
    }
    free(src);
    free(dest);

    struct stat fInfo;
    checkError(stat(srcPath,&fInfo)!=-1,"stat");
    int srcfd=open(srcPath,O_RDONLY);
    checkError(srcfd!=-1,"open");
    mode_t oldUmask = umask(0); //da bismo otvorili po zeljenim pravima pristupa
    int destfd =  open(destPath,O_WRONLY|O_CREAT|O_TRUNC);
    checkError(destfd!=-1,"open");
    umask(oldUmask);
    int readBytes=0;
    char buff[SIZE];
    while((readBytes=read(srcfd,buff,SIZE))>0){
        checkError(write(destfd,buff,readBytes)!=-1,"write");
    }
    checkError(readBytes!=-1,"read");
    close(srcfd);
    close(destfd);
}

void osMakePublicDir(const char* fpath){
    mode_t prava = 0777;
    mode_t oldUmask = umask(0);
    checkError(mkdir(fpath,prava)!=-1,"mkdir");
    umask(oldUmask);
}

bool osIsPublicFile(const char* fpath){
    struct stat fInfo;
    checkError(stat(fpath,&fInfo)!=-1,"stat");
    checkError(S_ISREG(fInfo.st_mode),"not regular");
    if((fInfo.st_mode & S_IROTH) && (fInfo.st_mode & S_IWOTH)){
        return true;
    }else{
        return false;
    }
}

int processFile(const char* fpath, const struct stat *sb, int typeflag, struct FTW* ftwbuf){
    
    if(typeflag==FTW_F){
        if(osIsPublicFile(fpath)){ //ako vazi da regularan fajl ima prava citanja i pisanja za oth
            unsigned days = osNumOfDaysFileModified(fpath);
            if(days<30){ //ukoliko su modifikovani u zadnjih 30 dana
                char *newpath = malloc(strlen(dirPath)+strlen(fpath+ftwbuf->base)+2); //offset koliko je daleko ime od pocetka putanje
                strcpy(newpath,dirPath);
                strcat(newpath,"/");
                strcat(newpath,fpath+ftwbuf->base);
                osMoveFile(fpath,newpath); //pomera ih u odgovarajuci direktorijum
                free(newpath);
            }else{
                checkError(unlink(fpath)!=-1,"unlink"); //brisemo
            }
        }
    }

    return 0;
}

int main(int argc, char **argv){

    checkError(argc==3,"args");
    dirPath = argv[2];
    osMakePublicDir(dirPath);
    checkError(nftw(argv[1],processFile,50,0)!=-1,"nftw");
    exit(EXIT_SUCCESS);
}
