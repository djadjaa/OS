#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <errno.h>

#define check_error(cond, msg) \
    do{ \
        if(!(cond)){ \
            exit(EXIT_FAILURE); \
        } \
    } while(0);

#define BUFF_LEN  (255)
#define ERR_LEN   (4094)
#define RD_END    (0)
#define WR_END    (1)

int main(int argc, char** argv){

    check_error(argc == 2, "Bad argumets");

    FILE* f = fopen(argv[1], "r");
    FILE* output = fopen("errors.txt", "w");

    check_error(f != NULL, "Failed to open file");

    char command_buffer[BUFF_LEN];
    char arg_buffer[BUFF_LEN];

    while(fscanf(f, "%s", command_buffer) != EOF) {
        fscanf(f, "%s", arg_buffer);

        //printf("%s %s\n", command_buffer, arg_buffer);

        int cld2par[2];
        check_error(pipe(cld2par) != -1, "Failed to create pipes");

        pid_t pid = fork();
        check_error(pid != -1, "Failed to fork proccess");

        char err_buffer[ERR_LEN + 1];

        if(pid > 0){
            close(cld2par[WR_END]);

            FILE* f_err = fdopen(cld2par[RD_END], "r");

            int i = 0;
            while((err_buffer[i] = getc(f_err)) != EOF){
                i++;
            }
            err_buffer[i] = '\0';

            fclose(f_err);
            close(cld2par[RD_END]);
        }
        else{

            close(cld2par[RD_END]);

            check_error(dup2(cld2par[WR_END], STDERR_FILENO) != -1, "Failed to redirect");

            check_error(
                execlp(command_buffer, command_buffer, arg_buffer, NULL) != -1,
                "Failed to exec command"
            );

            close(cld2par[WR_END]);
            exit(EXIT_SUCCESS);
        }

        int status = 0;
        check_error(wait(&status) != -1, "Failed to wait procces");

        if(WIFEXITED(status)){
            /*
            if(WEXITSTATUS(status) == EXIT_SUCCESS){
                printf("Chiled exit success \n");
            }
            */
            if(WEXITSTATUS(status) != 0){
                fprintf(output, "%s", err_buffer);
            }
        }
        else{
            printf("Child didnt exit\n");
        }
    }

    fclose(output);
    fclose(f);

    exit(EXIT_SUCCESS);
}
