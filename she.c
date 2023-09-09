#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX 1000

char** globalinputs; //declaring global variable to access in different functions //history
int globalcount;   //history

void show_history(){     //history
    for(int i=0;i<globalcount;i++){
        printf("%s \n",globalinputs[i]);
    }
    exit(0);
}


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
    while (tok != NULL ){            
        arr[arr_size++] = tok;
        tok = strtok(NULL, delimiter);
    }
    arr[arr_size++] = NULL;
    return arr_size;                                   //last is null 
}


int create_process_and_run(char* command) {
    char* arr[MAX];
    //int a_size = parse(command,arr);
    //printf("%s", arr[1]);
    int status = fork();
    if(status < 0) {
        printf("OS_A2@custom_shell:~$ Something bad happened\n");
    } 
    else if(status == 0) {
       char* command = read_user_input();  
       int arg_size = parse(command, arr, "|");                            
        if (arr==NULL){             //check for memory allocation
            printf("OS_A2@custom_shell:~$ Memory allocation failed!\n");
            exit(1);
        }
        
        // char* token = strtok(command, " | ");
        // printf("%s+ ", token);
                   //arr contains args, actual size is -1 as null separated
        
        // for (int i=0; i<arg_size; i++){
        //     printf("%s+ ", arr[i]);
        // }
        // printf("%d", arg_size);
        int fd[arg_size-1][2];
        for (int i=0; i<arg_size-1; i++){
            pipe(fd[i]);                                    //handle error here
        }
        if (arg_size - 1 == 1){            // this means that | delimiter doesnt exist
            char** args = (char**)malloc(MAX * sizeof(char*));
            parse(command, args, " ");
            globalinputs[globalcount]=args;   //history
            globalcount+=1;                 //history
            execvp(args[0],args);
            
        }

        else{
            for (int i=0; i<arg_size-1; i++){
                int stat = fork();
                if (stat < 0){
                    printf("Something bad happened.\n");
                }
                else if (stat == 0){
                    if (i != 0){         //reading from previous
                        close(fd[i-1][1]);
                        dup2(fd[i-1][0], STDIN_FILENO);
                        //close(fd[i-1][0]);
                    }
                    //writing to current

                    close(fd[i][0]);
                    dup2(fd[i][1], STDOUT_FILENO);
                    //close(fd[i][1]);

                    char** childargs = (char**)malloc(MAX * sizeof(char*));
                    int a1 = parse(arr[i], childargs, " ");
                    for (int i=0; i<a1; i++){
                            printf("%s+ ", childargs[i]);
                        }


                    globalinputs[globalcount]=childargs + fd[i-1][0];   //history
                    globalcount+=1;                 //history
                    execvp(childargs[0], childargs + fd[i-1][0]);
                    exit(0);
                }else{
                    if (i!=0){
                        close(fd[i-1][0]);
                    }
                    if (i == arg_size-2){
                        close(fd[i][1]);
                    }
                    wait(NULL);
                    
                }
            }
        }
        exit(0);

    } 
    else {
        wait(NULL);
        printf("OS_A2@custom_shell:~$ I am the parent Shell\n");
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
        if ((strcmp(command, "history") == 0)){    //history
            globalinputs[globalcount]="history";  //history
            globalcount+=1;    //history
            show_history(); //history
        }

        status = launch(command);
        free(command); // Free the dynamically allocated input
    } while (!status);
}



int main() {
    globalinputs=(char**)malloc(MAX*sizeof(char*)); //history
    globalcount=0;//history
    shell_loop();
    return 0;
}