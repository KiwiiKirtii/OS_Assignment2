#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX 1000

// Function to read user input (you need to implement this)
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

int parse(char * buff, char** arr){
    char* tok = strtok(buff," ");
    int arr_size = 0;
    while (tok != NULL){
        arr[arr_size++] = tok;
        tok = strtok(NULL, " ");
        //printf("%s {}", arr[arr_size-1]);
    }
    arr[arr_size++] = NULL;
    return arr_size;                                   //last is null 
}


int create_process_and_run(char* command, char** arr) {
    int a_size = parse(command,arr);
    int fd[2];
    pipe(fd);
    //printf("%s", arr[1]);
    int status = fork();
    if(status < 0) {
        printf("OS_A2@custom_shell:~$ Something bad happened\n");
    } 
    else if(status == 0) {
        close(fd[1]);
        printf("OS_A2@custom_shell:~$ I am the child process\n");
        exit(0);
    } 
    else {
        close(fd[0]);
        wait(NULL);
        printf("OS_A2@custom_shell:~$ I am the parent Shell\n");
    }
return 0;
}

int launch (char *command, char ** arr) {
    int status;
    status = create_process_and_run(command, arr);
    return status;
}

void shell_loop() {
    int status;
    do {
        char** arr = (char**)malloc(MAX * sizeof(char*));
        if (arr==NULL){             //check for memory allocation
        printf("OS_A2@custom_shell:~$ Memory allocation failed!\n");
        exit(1);
        }
        printf("OS_A2@custom_shell:~$ ");
        char* command = read_user_input();
        status = launch(command,arr);
        free(command); // Free the dynamically allocated input
        free(arr);
        //status = 1;
    } while (!status);
}


int main() {
    shell_loop();
    return 0;
}