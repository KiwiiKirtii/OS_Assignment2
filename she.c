#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX 1000
#include <time.h>

char** globalinputs; //declaring global variable to access in different functions //history
int globalcount = 0;   //histoy
int globalstart,globalend,globalpid,globalhcount; //time
// int globalend;
// int globalpid;
int ** globalstarttime;  //time
int ** globalruntime;
int**globalprocessid;
time_t** globaltime[20];
time_t globalcurrenttime;
// int globalhcount;

void show_history(){     //history
    for(int i=0;i<globalcount;i++){
        printf("%d. %s \n", i+1, globalinputs[i]);
    }
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

    globalinputs[globalcount++] = strdup(command);

    int arg_size = parse(command, arr, "|");
    // for (int j=0; j<arg_size; j++){
    //     globalinputs[globalcount] = strdup(arr[j]);
    //     globalcount+=1;
    // }
    if (arg_size == 1) {
        // If there's only one command, no need for pipes
        int status = fork();
        if (status == 0) {
            char** args = (char**)malloc(MAX * sizeof(char*));
            parse(arr[0], args, " ");
            //globalinputs[globalcount]=arr[0];   //history
            //globalcount+=1;                 //history
            
            execvp(args[0], args);
            perror("execvp");
            exit(1);
        } else if (status < 0) {
            printf("Something bad happened.\n");
        } else {
            globalcurrenttime=time(NULL);
            globaltime[globalhcount]=globalcurrenttime;
            globalstart=clock();     //time
            globalprocessid[globalhcount]=wait(NULL);  //pid
            globalend=clock();
            globalruntime[globalhcount]=globalend-globalstart; //time
            globalstarttime[globalhcount]=globalstart;
            globalhcount+=1;
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
                //globalinputs[globalcount]=arr[i];   //history
                //globalcount+=1;                 //history
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
                    globalcurrenttime=time(NULL);
                    globaltime[globalhcount]=globalcurrenttime;
                    globalstart=clock();
                    globalstarttime[globalhcount]=globalstart;
                    close(pipes[i][0]);  // Close the read end of the last pipe
                    close(pipes[i][1]);  // Close the write end of the last pipe
                    globalprocessid[globalhcount]=wait(NULL);
                    globalend=clock();
                    globalruntime[globalhcount]=globalend-globalstart;
                    globalhcount+=1;
                    
                }
                else{
                    wait(NULL);
                }
                

            } else {
                printf("Something bad happened.\n");
            }
        }
    }

    return 0;
}

void process_info(){
    printf("Command Name                    PID              Start Time           Run Time\n");
    for (int i=0;i<globalhcount;i++){
        printf("%s                    %d              %ld           %d\n",globalinputs[i],globalprocessid[i],globaltime[i],globalruntime[i]);
    }
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
        if ((strcmp(command, "history")) != 0){
            status = launch(command);
        }
        
        else if ((strcmp(command, "history") == 0)){    //history
            globalcurrenttime=time(NULL);
            globaltime[globalhcount]=globalcurrenttime;
            globalstart=clock();
            globalstarttime[globalhcount]=globalstart;
            globalinputs[globalcount]="history";  //history
            globalcount+=1;    //history
            show_history(); //history
            globalprocessid[globalhcount]=NULL;
            globalend=clock();
            globalruntime[globalhcount]=globalend-globalstart;
            globalhcount+=1;
            process_info();
        }
        free(command); // Free the dynamically allocated input
    } while (!status);
}


int main() {
    globalend=0;
    globalstart=0; //time
    globalpid=0;
    globalcount=0;
    globalstarttime=(int**)malloc(MAX*sizeof(int*));
    globalruntime=(int**)malloc(MAX*sizeof(int*));
    globalprocessid=(int**)malloc(MAX*sizeof(int*));
    globalinputs=(char**)malloc(MAX*sizeof(char*)); //history
    //globaltime=(time_t**)malloc(MAX*sizeof(time_t*));
    globalcurrenttime=time(NULL);
    shell_loop();
    return 0;
}