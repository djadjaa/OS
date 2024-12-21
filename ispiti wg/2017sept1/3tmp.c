#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <float.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/epoll.h>

#define check_error(cond, msg) \
    do { \
        if(!(cond)) { \
            perror(msg); \
            exit(EXIT_FAILURE); \
        } \
    } while(0);

typedef struct {

    int ind;
    int fd;
    char* path;
} epoll_dt;

#define MAX_TRIG (20)

int main(int argc, char** argv) {

    check_error(argc > 1, "Bad args");

    int n_events = argc - 1;

    int epoll_fd = epoll_create(n_events);
    check_error(epoll_fd != -1, "Failed to create epoll fd");

    struct epoll_event* events =
        (struct epoll_event*) malloc(n_events * sizeof(struct epoll_event));

    check_error(events != NULL, "Failed to allocate events array");

    for(int i = 0 ; i < n_events; i ++) {

        int fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(fd != -1, "Failed to open file");

        epoll_dt* data = (epoll_dt*) malloc(sizeof(epoll_dt));

        data->ind = i;
        data->fd = fd;
        data->path = argv[i + 1];

        events[i].events = EPOLLIN | EPOLLHUP | EPOLLERR;
        events[i].data.ptr = data;

        check_error(
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &events[i]) != -1,
            "Failed to add epoll event");
    }

    int active_events = n_events;
    struct epoll_event trigg_events[MAX_TRIG];

    float max_sum = 0;
    char* max_path;

    while(active_events) {

        int n_trigg = epoll_wait(epoll_fd, trigg_events, MAX_TRIG, -1);
        check_error(n_trigg != -1, "Failed to wait epoll event");

        for(int i = 0; i < n_trigg; i ++) {

            if(trigg_events[i].events & EPOLLIN) {

                epoll_dt* data = (epoll_dt*)trigg_events[i].data.ptr;

                FILE* f = fdopen(data->fd, "r");

                float local_sum = 0;
                float x;
                while(fscanf(f, "%f", &x) != EOF) {
                    local_sum += x;
                }

                if(max_sum == 0 || local_sum > max_sum) {
                    max_sum = local_sum;
                    max_path = data->path;
                }

            }
            else if(trigg_events[i].events & (EPOLLHUP | EPOLLERR)) {

                epoll_dt* data = (epoll_dt*)trigg_events[i].data.ptr;

                check_error(
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, data->fd, &trigg_events[i]) != -1,
                    "Failed to delete epoll");

                close(data->fd);

                active_events --;

                free(data);
            }
        }
    }

    printf("%s\n", max_path);

    exit(EXIT_SUCCESS);
}
