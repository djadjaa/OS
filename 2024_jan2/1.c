/*

.. - prethodni dir
. - tekuci dir
*/

#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <utime.h>

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

time_t max(time_t a, time_t b){
    return (a > b) ? a : b;
}

int main(int argc, char **argv){

    DIR *folder = opendir(".");
    checkError(folder!=NULL,"opendir");
    struct dirent *fajlUFolderu;
    time_t maxATime = 0;
    time_t maxMTime = 0;
    while ((fajlUFolderu=readdir(folder))!=NULL){
        struct stat fileInfo;
        checkError(stat(fajlUFolderu->d_name, &fileInfo)==0,"stat");
        maxATime = max(maxATime, fileInfo.st_atime);
        maxMTime = max(maxMTime, fileInfo.st_mtime);
    }
    closedir(folder);
    //printf("%ld %ld\n", maxATime, maxMTime);

    folder = opendir(".");
    checkError(folder!=NULL,"opendir");

    struct utimbuf newTime={
        .actime = maxATime,
        .modtime = maxMTime,
    };
    
    while ((fajlUFolderu=readdir(folder))!=NULL){
        checkError(utime(fajlUFolderu->d_name,&newTime)==0,"utime");
    }

    closedir(folder);
    exit(EXIT_SUCCESS);
}
