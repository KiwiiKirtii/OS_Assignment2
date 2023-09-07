#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX 1000

// Function to read user input (you need to implement this)
char* read_user_input() {
    char* buffer = (char*)malloc(MAX * sizeof(char));
    // Read user input dynamically
    fgets(buffer, MAX, stdin);
    
    return buffer;
}


int create_process_and_run(char* command) {
    int status = fork();
    if(status < 0) {
    printf("Something bad happened\n");
    } else if(status == 0) {
    printf("I am the child process\n");
    } else {
    printf("I am the parent Shell\n");
    }
return 0;
}

int launch (char *command) {
    int status;
    status = create_process_and_run(command);
    return status;
}

void shell_loop() {
    int status;
    do {
        printf("OS_A2@custom_shell:~$ ");
        char* command = read_user_input();
        status = launch(command);
        free(command); // Free the dynamically allocated input
    } while (!status);
}

int main() {
    shell_loop();
    return 0;
}