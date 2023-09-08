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
    char** arr = (char**)malloc(MAX * sizeof(char*));
    if (arr == NULL) {
        printf("OS_A2@custom_shell:~$ Memory allocation failed!\n");
        exit(1);
    }

    int arg_size = parse(command, arr, "|");
    if (arg_size == 1) {
        // If there's only one command, no need for pipes
        int status = fork();
        if (status == 0) {
            char** args = (char**)malloc(MAX * sizeof(char*));
            parse(arr[0], args, " ");
            execvp(args[0], args);
            perror("execvp");
            exit(1);
        } else if (status < 0) {
            printf("Something bad happened.\n");
        } else {
            wait(NULL);
        }
    } else {
        int pipes[arg_size][2];  // Create an array of pipes

        for (int i = 0; i < arg_size; i++) {
            if (i < arg_size - 1) {
                if (pipe(pipes[i]) == -1) {
                    perror("pipe");
                    exit(1);
                }
            }

            int stat = fork();
            if (stat == 0) {
                // Child process
                if (i > 0) {
                    close(pipes[i - 1][1]);  // Close the write end of the previous pipe
                    dup2(pipes[i - 1][0], STDIN_FILENO);  // Redirect stdin from the previous pipe
                    close(pipes[i - 1][0]);
                }
                if (i < arg_size - 1) {
                    close(pipes[i][0]);  // Close the read end of the current pipe
                    dup2(pipes[i][1], STDOUT_FILENO);  // Redirect stdout to the current pipe
                    close(pipes[i][1]);
                }
                // if (i == arg_size -1){
                //     close(pipes[i][0]);
                // }

                char** args = (char**)malloc(MAX * sizeof(char*));
                parse(arr[i], args, " ");
                execvp(args[0], args);
                perror("execvp");
                exit(1);
            } else if (stat > 0) {
                // Parent process
                if (i > 0) {
                    close(pipes[i - 1][0]);  // Close the read end of the previous pipe
                    close(pipes[i - 1][1]);  // Close the write end of the previous pipe
                }

                // In the last iteration, wait for the last child process to complete
                if (i == arg_size - 1) {
                    close(pipes[i][0]);  // Close the read end of the last pipe
                    close(pipes[i][1]);  // Close the write end of the last pipe
                    
                }
                wait(NULL);
            } else {
                printf("Something bad happened.\n");
            }
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