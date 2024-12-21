#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while (0)
    
int suma = 0;
 
int processFile(const char* putanja, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
     
     if(typeflag == FTW_F){
         
         if((sb -> st_mode & S_IXUSR) || (sb ->st_mode & S_IXGRP) || (sb -> st_mode & S_IXOTH)){
             
             suma += (int)sb -> st_size;
         }
     }
         return 0;
         
     
 }
    
    
int main (int argc, char** argv){
    
    
    check_error(argc == 2, "...");
    
    suma = 0;
    
    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "...");
    check_error(S_ISDIR(fInfo.st_mode), "...");
    
    check_error(nftw(argv[1], processFile, 50, 0) != -1, "...");
    
    printf("%d\n", (int)suma);
    exit(EXIT_SUCCESS);
    
}
    