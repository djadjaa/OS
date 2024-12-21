#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <libgen.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#define check_error(cond,msg) \
    do { \
        if(!(cond)) { \
            perror(msg); \
            exit(EXIT_FAILURE); \
        } \
    } while(0);

void print_dir(char* path, bool should_print_files);
bool should_print_files_f(char* path);

int main(int argc, char** argv) {

    // argv[1] - naziv direktorijuma koji treba obici

    check_error(argc == 2, "Bad args");

    struct stat arg_stat;
    check_error(stat(argv[1], &arg_stat) != -1, "Failed to stat");
    check_error(S_ISDIR(arg_stat.st_mode) != 0, "Bad argv type");

    print_dir(argv[1], should_print_files_f(argv[1]));

    exit(EXIT_SUCCESS);
}

void print_dir(char* path, bool should_print_files) {

    DIR* dir = opendir(path);
    check_error(dir != NULL, "Failed top open dir");

    check_error(chdir(path) != -1, "Failed to change dir");

    struct dirent* current_dir;
    while((current_dir = readdir(dir)) != NULL) {

        struct stat f_info;
        check_error(lstat(current_dir->d_name, &f_info) != -1, "Failed to stat");

        //printf("%s\n", current_dir->d_name);

        if(S_ISREG(f_info.st_mode)) {

            if(should_print_files) 
                printf("%s\n", current_dir->d_name);
        }
        else if(S_ISDIR(f_info.st_mode)) {

            if(strcmp(current_dir->d_name, ".") != 0 && strcmp(current_dir->d_name, "..") != 0){

                print_dir(current_dir->d_name, should_print_files_f(current_dir->d_name) || should_print_files);
            }
        }
    }

    closedir(dir);

    check_error(chdir("..") != -1, "Failed to change dir");
}


bool should_print_files_f(char* path) {

    char* dirname = basename(path);

    //printf("%s\n", dirname);

    char name_prefix[5] = {dirname[0], dirname[1], dirname[2], dirname[3], '\0'};

    return strcmp(name_prefix, "dir_") == 0;
}
