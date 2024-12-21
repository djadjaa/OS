#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LEN

#define check_error(cond, msg) \
    do { \
        if(!(cond)) { \
            perror(msg); \
            exit(EXIT_FAILURE); \
        } \
    } while (0);

int is_unlocked(int, int, int);
char* reverse(char*,int);
void mod(int,int, char*, int);
void test_lock(int);

int main(int argc, char** argv) {

    check_error(argc == 2, "Bad args");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "Failed to open file");

    //test_lock(fd);

    int uspeh = 0;
    int neuspeh = 0;

    int file_pt = 0;
    int word_pt = 0;
    int word_start = 0;
    int word_len = 0;

    char word[MAX_LEN + 1];
    char c;

    while(read(fd, &c, 1) > 0) {

        if(c == ' ' || c == '\n') {

            word[word_pt] = '\0';

            //printf("%s %d %d ", word, word_start, word_len);

            if(is_unlocked(fd, word_start, word_len)) {

                //printf("unlocked ");

                char* reverse_word = reverse(word, word_len);
                mod(fd, word_start, reverse_word, word_len);

                //printf("%s\n", reverse_word);

                free(reverse_word);

                uspeh += 1;
            }
            else {
                //printf("locked\n");

                neuspeh += 1;
            }

           word_start = file_pt + 1;
           word_len = 0;
           word_pt = 0;

        }
        else {

            word[word_pt] = c;
            word_pt += 1;
            word_len += 1;
        }
        file_pt += 1;
    }

    printf("%d %d\n", uspeh, neuspeh);

    close(fd);
    exit(EXIT_SUCCESS);

}

int is_unlocked(int fd, int start, int len) {

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;

    check_error(fcntl(fd, F_GETLK, &lock) != -1, "Failed to fcntl");

    if(lock.l_type == F_UNLCK) return 1;
    else return 0;
}

char* reverse(char* buffer,int len) {

    char* ret_val = (char*) malloc ((len + 1) * sizeof(char));

    int i = len - 1;
    int j = 0;
    while(i >= 0) {
        ret_val[j] = buffer[i];
        i --;
        j ++;
    }
    ret_val[j] = '\0';

    return ret_val;

}

void mod(int fd, int start, char* buffer, int len) {

    lseek(fd, start, SEEK_SET);

    int i = 0;
    write(fd, buffer, len);

    lseek(fd, 1, SEEK_CUR);
}
