#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX 1000

// Function to read user input
char* read_user_input() {
    char* buffer = (char*)malloc(MAX * sizeof(char));
    // Read user input dynamically

    if (buffer==NULL){             //check for memory allocation
        printf("OS_A2@custom_shell:~$ Memory allocation failed!\n");
        exit(1);
    }

    if (fgets(buffer, MAX, stdin)==NULL){   //check for reading user input
        printf("OS_A2@custom_shell:~$ Could not read input\n");
        exit(1);
    }
    int length = strlen(buffer);


    for (int i=0;i<length;i++){        //check for backslash and quotes
        if (buffer[i]== '\\' || buffer[i]=='"'){
            printf("OS_A2@custom_shell:~$ Invalid Input, contains backslashes or quotes!\n");
            exit (1);
        }
    }

    // for (int i=0;i<length;i++){   //replacing new line character
    //     if (buffer[length-1]=="\n"){
    //         buffer[length-1]="\0";
    //     }
    // }

    buffer[strcspn(buffer, "\n")] = '\0';
    return buffer;
}

int parse(char * buff, char** arr, char* delimiter){
    char* tok = strtok(buff,delimiter);
    int arr_size = 0;
    while (tok != NULL ){            //////////////////////////////////////////
        arr[arr_size++] = tok;
        tok = strtok(NULL, delimiter);
    }
    arr[arr_size] = NULL;
    return arr_size;                                   //last is null but not added to returned size
}


int create_process_and_run(char* command) {
    char* arr[MAX];
    int arr_size = parse(command, arr, "|");

    int prev_pipe[2];
    int curr_pipe[2];

    for (int i = 0; i < arr_size; i++) {
        // Create a new pipe for the current command
        if (i < arr_size - 1) {                         //dont need for last
            if (pipe(curr_pipe) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        int status = fork();
        if (status == -1) {
            perror("fork");
            exit(1);
        } else if (status == 0) { // Child process
            // Close unnecessary pipe ends
            if (i > 0) {
                close(prev_pipe[1]); // Close write end of the previous pipe
                dup2(prev_pipe[0], STDIN_FILENO); // Connect stdin to the read end of the previous pipe
                close(prev_pipe[0]);
            }
            if (i < arr_size - 1) {
                close(curr_pipe[0]); // Close read end of the current pipe
                dup2(curr_pipe[1], STDOUT_FILENO); // Connect stdout to the write end of the current pipe
                close(curr_pipe[1]);
            }

            // Execute the command
            char** args = (char**)malloc(MAX * sizeof(char*));
            int arg_size = parse(arr[i], args, " ");
            execvp(args[0], args);
            perror("execvp");
            exit(1);
        } else { // Parent process
            // Close unnecessary pipe ends
            if (i > 0) {
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }
            if (i < arr_size - 1) {
                prev_pipe[0] = curr_pipe[0];
                prev_pipe[1] = curr_pipe[1];
            }
            wait(NULL); // Wait for the child process to complete
        }
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