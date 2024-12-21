#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

#define MAX_PATH_LEN 256

// Globalna suma dužina reči
int global_sum = 0;
pthread_mutex_t mutex;

// Struktura za prosleđivanje argumenata niti
typedef struct {
    char filepath[MAX_PATH_LEN];
} thread_arg;

// Funkcija koja računa sumu dužina reči u fajlu
void* calculate_word_lengths(void* arg) {
    thread_arg* t_arg = (thread_arg*)arg;
    FILE* file = fopen(t_arg->filepath, "r");

    if (file == NULL) {
        perror("Greska pri otvaranju fajla");
        pthread_exit(NULL);
    }

    int local_sum = 0;
    char word[256];

    // Čitanje svake reči iz fajla i računanje njene dužine
    while (fscanf(file, "%s", word) != EOF) {
        local_sum += strlen(word);
    }

    fclose(file);

    // Ažuriranje globalne sume uz sinhronizaciju preko muteksa
    pthread_mutex_lock(&mutex);
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {
    pthread_mutex_init(&mutex, NULL);

    char filepath[MAX_PATH_LEN];
    pthread_t threads[100];
    thread_arg args[100];
    int thread_count = 0;

    // Učitavanje putanja sa standardnog ulaza i kreiranje niti
    while (scanf("%s", filepath) != EOF) {
        strcpy(args[thread_count].filepath, filepath);
        pthread_create(&threads[thread_count], NULL, calculate_word_lengths, &args[thread_count]);
        thread_count++;
    }

    // Čekanje svih niti da završe
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    // Ispis rezultata
    printf("Ukupna suma duzina reci: %d\n", global_sum);

    pthread_mutex_destroy(&mutex);
    return 0;
}
