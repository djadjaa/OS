#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

int main(int argc, char **argv){

    checkError(argc==3,"args");
    struct stat fInfo;
    checkError(stat(argv[1],&fInfo)!=-1,"stat");
    //argv[2][0] argv[2][1] argv[2][2]
    if(argv[2][0]=='u'){
        if(argv[2][1]=='+'){
            if(argv[2][2]=='r')
                fInfo.st_mode |= S_IRUSR;
            else if(argv[2][2]=='w')
                fInfo.st_mode |= S_IWUSR;
            else if(argv[2][2]=='x')
                fInfo.st_mode |= S_IXUSR;
            else
                checkError(0,"wrong op");
        }
        else if(argv[2][1]=='-'){
            if(argv[2][2]=='r')
                fInfo.st_mode &= ~S_IRUSR;
            else if(argv[2][2]=='w')
                fInfo.st_mode &= ~S_IWUSR;
            else if(argv[2][2]=='x')
                fInfo.st_mode &= ~S_IXUSR;
            else
                checkError(0,"wrong op");
        }
        else{
            checkError(0,"wrong op");
        }
    }else if(argv[2][0]=='g'){
        if(argv[2][1]=='+'){
            if(argv[2][2]=='r')
                fInfo.st_mode |= S_IRGRP;
            else if(argv[2][2]=='w')
                fInfo.st_mode |= S_IWGRP;
            else if(argv[2][2]=='x')
                fInfo.st_mode |= S_IXGRP;
            else
                checkError(0,"wrong op");
        }
        else if(argv[2][1]=='-'){
            if(argv[2][2]=='r')
                fInfo.st_mode &= ~S_IRGRP;
            else if(argv[2][2]=='w')
                fInfo.st_mode &= ~S_IWGRP;
            else if(argv[2][2]=='x')
                fInfo.st_mode &= ~S_IXGRP;
            else
                checkError(0,"wrong op");
        }
        else{
            checkError(0,"wrong op");
        }
    }else if(argv[2][0]=='o'){
        if(argv[2][1]=='+'){
            if(argv[2][2]=='r')
                fInfo.st_mode |= S_IROTH;
            else if(argv[2][2]=='w')
                fInfo.st_mode |= S_IWOTH;
            else if(argv[2][2]=='x')
                fInfo.st_mode |= S_IXOTH;
            else
                checkError(0,"wrong op");
        }
        else if(argv[2][1]=='-'){
            if(argv[2][2]=='r')
                fInfo.st_mode &= ~S_IROTH;
            else if(argv[2][2]=='w')
                fInfo.st_mode &= ~S_IWOTH;
            else if(argv[2][2]=='x')
                fInfo.st_mode &= ~S_IXOTH;
            else
                checkError(0,"wrong op");
        }
        else{
            checkError(0,"wrong op");
        }
    }else{
        checkError(0,"wrong op");
    }
    checkError(chmod(argv[1],fInfo.st_mode)!=-1,"chmod");
    exit(EXIT_SUCCESS);
}