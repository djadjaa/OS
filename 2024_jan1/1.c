#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#define checkError(expr,usrMsg) \
    do{ \
        if (!(expr)){ \
            perror(usrMsg); \
            exit(1); \
        } \
    }while(0)

int main(int argc, char **argv){

    checkError(argc >= 2, "args");
    struct stat fileInfo;
    off_t minSize=-1;
    char *pathname = NULL;
    char *final = NULL;
    for(int i=1; i<argc; i++){
        checkError(stat(argv[i], &fileInfo)!=-1,"stat"); 
        pathname = basename(argv[i]);
        if(minSize == -1 || fileInfo.st_size < minSize){
            minSize = fileInfo.st_size;
            if (final != NULL) {
                free(final);
            }

            final = malloc((strlen(pathname) + 1) * sizeof(*pathname));
            checkError(final != NULL, "malloc");

            strcpy(final, pathname);
            /*
            strdup(final,pathname);
            checkError(final != NULL, "malloc");
            */
        }
    }

    printf("%s\n", final);
    free(final);
    exit(EXIT_SUCCESS);
}