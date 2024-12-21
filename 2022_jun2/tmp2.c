#define _XOPEN_SOURCE (700)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define provera_greske(uslov, poruka) \
    do { \
        if(!(uslov)) { \
            fprintf(stderr, "%s\n", poruka); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)
 
#define MAX (257)

int main(int argc, char** argv) {
    
    provera_greske(argc == 2, "");

    struct stat f_info;
    provera_greske(stat(argv[1], &f_info) != -1, "");

    int fd = open(argv[1], O_RDWR);
    provera_greske(fd != -1, "");

    FILE* ulaz = fdopen(fd, "r+");
    provera_greske(ulaz != NULL, "");

    char* pocetak = "_test";
    int n = strlen(pocetak);

    char rec[MAX];
    while(fscanf(ulaz, "%s", rec) == 1) {
        if(strncmp(rec, pocetak, n) == 0) {
            struct flock katanac;
            int m = strlen(rec);
            katanac.l_start = ftell(ulaz);
            katanac.l_type = F_WRLCK;
            katanac.l_whence = SEEK_SET;
            katanac.l_len = -m;

            int rez = fcntl(fd, F_SETLK, &katanac);
            if(rez == -1) {
                if(errno != EACCES && errno != EAGAIN) {
                    provera_greske(0, "");
                }
            } else {
                provera_greske(fseek(ulaz, -m, SEEK_CUR) != -1, "");
                for(int i = 0; rec[i] != '\0'; i++) {
                    rec[i] = '#';
                }
                fprintf(ulaz, "%s", rec);
                katanac.l_type = F_UNLCK;
                provera_greske(fcntl(fd, F_SETLK, &katanac) != -1, "");
            }
        }
    }

    fclose(ulaz);
    exit(EXIT_SUCCESS);
} 