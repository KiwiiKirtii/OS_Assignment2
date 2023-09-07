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
    int length=sizeof(buffer);

    if (buffer==NULL){             //check for memory allocation
        printf("memory allocation failed!!! \n");
        exit(1);
    }

    if (fgets(buffer, MAX, stdin)==NULL){   //check for reading user input
        printf("could not read user code!!\n");
        exit(1);
    }

    for (int i=0;i<length;i++){        //check for backslash and quotes
        if (buffer[i]=='\"'|| buffer[i]=="'"|| buffer[i]=='""'){
            printf("invalid input contains backslashes or quotes!! \n");
            exit (1);
        }
    }

    for (int i=0;i<length;i++){   //replacing new line character
        if (buffer[length-1]=="\n"){
            buffer[length-1]="\O";
        }
    }
 
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