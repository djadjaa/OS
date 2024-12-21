#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <seconds since epoch>\n", argv[0]);
        return 1;
    }

    // Parse input argument
    time_t input_time = atol(argv[1]);

    // Get the current time
    struct tm *input_tm = localtime(&input_time);
    if (input_tm == NULL) {
        perror("localtime");
        return 1;
    }

    // Set day to 1 and time to 00:00:00 to get the start of the current month
    struct tm start_of_month = *input_tm;
    start_of_month.tm_mday = 1;
    start_of_month.tm_hour = 0;
    start_of_month.tm_min = 0;
    start_of_month.tm_sec = 0;

    // Convert the start of the month to time_t
    time_t start_of_month_time = mktime(&start_of_month);
    if (start_of_month_time == -1) {
        perror("mktime");
        return 1;
    }

    // Calculate the difference in seconds and then convert to hours
    double difference_in_seconds = difftime(input_time, start_of_month_time);
    double difference_in_hours = difference_in_seconds / 3600;

    // Print the result
    printf("Number of hours from the start of the current month: %.2f\n", difference_in_hours);

    return 0;
}
