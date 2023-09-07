#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 1000

// Function to read user input (you need to implement this)
char* read_user_input() {
    char* buffer = (char*)malloc(MAX * sizeof(char));;
    
    printf("OS_A2@custom_shell:~$ ");
    
    // Read user input dynamically
    fgets(buffer, MAX, stdin);
    
    if (read == -1) {
        printf("getline");
        exit(EXIT_FAILURE);
    }
    
    // Remove newline character
    if (input[read - 1] == '\n') {
        input[read - 1] = '\0';
    }
    
    return buffer;
}

int launch (char *command) {
    int status;
    status = create_process_and_run(command);
    return status;
}


int create_process_and_run(char* command) {
    pid_t child_pid = fork();
    
    if (child_pid < 0) {
        printf("fork");
        return 1; // Error
    } else if (child_pid == 0) { // Child process
        printf("I am the child process\n");
        
        // Parse the command into arguments
        char* args[64]; // Adjust the array size as needed
        char* token = strtok(command, " ");
        int arg_count = 0;
        
        while (token != NULL) {
            args[arg_count] = token;
            arg_count++;
            token = strtok(NULL, " ");
        }
        
        args[arg_count] = NULL; // Null-terminate the argument list
        
        // Execute the command in the child process
        execvp(args[0], args);
        
        // If execvp fails, print an error message and exit
        printf("execvp");
        exit(1); // Exit the child process with an error code
    } else { // Parent process
        printf("I am the parent Shell\n");
        
        // Wait for the child process to finish
        int child_status;
        waitpid(child_pid, &child_status, 0);
        
        // Check if the child process exited normally
        if (WIFEXITED(child_status)) {
            int exit_status = WEXITSTATUS(child_status);
            printf("Child process exited with status: %d\n", exit_status);
        } else if (WIFSIGNALED(child_status)) {
            int termination_signal = WTERMSIG(child_status);
            printf("Child process terminated by signal: %d\n", termination_signal);
        }
    }
    
    return 0; // Success
}



void shell_loop() {
    int status;
    do {
        char* command = read_user_input();
        status = launch(command);
        free(command); // Free the dynamically allocated input
    } while (status);
}

int main() {
    shell_loop();
    return 0;
}
