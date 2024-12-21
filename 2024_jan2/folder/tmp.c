#define _XOPEN_SOURCE (700)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <ftw.h>

#define provera_greske(uslov, poruka) \
    do { \
        if(!(uslov)) { \
            fprintf(stderr, "%s\n", poruka); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

#define RD_END (0)
#define WR_END (1)
#define MAX (2)
#define MAX_KOMANDE (256)
#define MAX_BAF (1024)

char** alociraj_komandu(char* linija) {
    
    int br_reci_komande = 0;
    int ukupno_procitano = 0;
    int procitano_trenutno = 0;
    sscanf(linija, "%d%n", &br_reci_komande, &procitano_trenutno);
    ukupno_procitano += procitano_trenutno;

    char** komande = (char**)malloc((br_reci_komande+2)*sizeof(char*));
    provera_greske(komande != NULL, "");

    char komanda[MAX_KOMANDE];
    sscanf(linija + ukupno_procitano, "%s%n", komanda, &procitano_trenutno);
    ukupno_procitano += procitano_trenutno;

    komande[0] = (char*)malloc((strlen(komanda) + 1)*sizeof(char));
    provera_greske(komande[0] != NULL, "");
    strcpy(komande[0], komanda);

    komande[1] = (char*)malloc((strlen(komanda) + 1)*sizeof(char));
    provera_greske(komande[1] != NULL, "");
    strcpy(komande[1], komanda);

    for(int i = 2; i <= br_reci_komande; i++) {
        sscanf(linija + ukupno_procitano, "%s%n", komanda, &procitano_trenutno);
        ukupno_procitano += procitano_trenutno;
        komande[i] = (char*)malloc((strlen(komanda) + 1)*sizeof(char));
        provera_greske(komande[i] != NULL, "");
        strcpy(komande[i], komanda);
    }
    komande[br_reci_komande+1] = NULL;

    return komande;
}

void obrisi_komandu(char** komanda) {
    for(int i = 0; komanda[i] != NULL; i++) {
        free(komanda[i]);
    }
    free(komanda);
}
 
int main(int argc, char** argv) {

    provera_greske(argc == 2, "");
    
    FILE* ulaz = fopen(argv[1], "r");
    provera_greske(ulaz != NULL, "");

    char max_komanda[MAX_KOMANDE];
    int max_broj = -1;

    int dete_rod[MAX];

    char* linija = NULL;
    size_t n = 0;
    while(getline(&linija, &n, ulaz) > 0) {

        char** komande = alociraj_komandu(linija);

        provera_greske(pipe(dete_rod) != -1, "");
        pid_t dete_pid = fork();
        provera_greske(dete_pid != -1, "");

        if(dete_pid == 0) {
            close(dete_rod[RD_END]);

            provera_greske(dup2(dete_rod[WR_END], STDERR_FILENO) != -1, "");
            provera_greske(execvp(komande[0], komande+1) != -1, "");   
        }

        int status = 0;
        provera_greske(wait(&status) != -1, "");

        close(dete_rod[WR_END]);

        if(!(WIFEXITED(status) && (WEXITSTATUS(status) == EXIT_SUCCESS))) {
            char bafer[MAX_BAF];
            int procitano = 0;
            int uk_pr = 0;
            while((procitano = read(dete_rod[RD_END], bafer, MAX_BAF)) > 0) {
                uk_pr += procitano;
            } 
            provera_greske(procitano != -1, "");
            
            if(uk_pr> max_broj) {
                max_broj = uk_pr;
                strcpy(max_komanda, komande[0]);
            }
        } 

        close(dete_rod[RD_END]);
        obrisi_komandu(komande);
    }

    if(max_broj != -1) {
        fprintf(stdout, "%s %d\n", max_komanda, max_broj);
    } else {
        fprintf(stdout, "unknown 0\n");
    }
 
    fclose(ulaz);
    free(linija);
    exit(EXIT_SUCCESS);
} 