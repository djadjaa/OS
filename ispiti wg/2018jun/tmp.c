#define _GNU_SOURCE (500)
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <signal.h>
#include <semaphore.h>

#define osAssert(cond, msg) osFattalError(cond, msg, __FILE__, __LINE__)
#define osPthreadAssert(cond, msg) osPthreadFattalError(cond, msg, __FILE_, __LINE__)
#define ARRAY_MAX (1024)

void osFattalError(int cond, const char *msg, const char *file, int line)
{
    if(!cond)
    {
        perror(msg);
        fprintf(stderr, "%s:%d\n", file, line);
        exit(EXIT_FAILURE);
    }
}

void osPthreadFattalError(int retval , const char *msg, const char *file, int line)
{
    if(retval)
    {
        fprintf(stderr, "%s:%s\n%s:%d\n", strerror(retval), msg, file, line);
        exit(EXIT_FAILURE);
    }
}

#define RD_END (0)
#define WR_END (1)

int main(int argc, char **argv)
{
    int pipeFds[2];
    osAssert(-1 != pipe(pipeFds), "pipe failed");
    pid_t child = fork();
    osAssert(-1 != child, "fork failed");

    if(child > 0) //parent
    {
        close(pipeFds[WR_END]);
        FILE *f = fdopen(pipeFds[RD_END], "r");
        osAssert(NULL != f, "fdopen failed");

        char *line = NULL;
        size_t len = 0;
        int status;
        char prava_pristupa[256] = {0};
        char naziv[256] = {0};

        int retval = getline(&line, &len, f);
        //ignorisem prvu liniju
        line = NULL;
        len = 0;

        osAssert(-1 != wait(&status), "wait failed");

        if(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
        {
                errno = 0;
                while(-1 != getline(&line, &len, f))
                {
                    sscanf(line , "%s", prava_pristupa);
                    //printf("%s\n", prava_pristupa);
                    if(!strncmp(prava_pristupa+7,"rwx",3))
                    {
                        sscanf(line, "%*s%*s%*s%*s%*s%*s%*s%*s%s", naziv);
                        printf("%s\n", naziv);
                    }
                }
                osAssert(0 == errno, "getline failed");
                free(line);
        }
        else
        {
            printf("neuspeh\n");
            return WEXITSTATUS(status);
        }
        free(line);
        fclose(f);
    }
    else //child
    {
        close(pipeFds[RD_END]);
        osAssert(-1 != dup2(pipeFds[WR_END], STDOUT_FILENO), "dup2 failed");
        close(pipeFds[WR_END]);
        osAssert(-1 != execlp("ls", "ls", "-l", (char*)NULL), "execlp failed");
        exit(0);
    }

    return 0;
}
