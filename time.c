#include <stdio.h>
#include <time.h>

int main() {
    // Record the start time
    clock_t start_time = clock();

    // Perform some time-consuming operation (e.g., a loop)
    for (int i = 0; i < 200; i++) {
        // Some time-consuming task
        printf("%s\n","kirti");
    }

    // Record the end time
    clock_t end_time = clock();
    
    
    // Calculate the elapsed time in seconds
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Elapsed time: %f seconds\n", elapsed_time);

    return 0;
}
