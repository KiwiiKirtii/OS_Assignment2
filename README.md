# OS_Assignment2
SIMPLE SHELL README (Group - 60)
Simple shell has been made and bonus 3.b. is implemented.
(Bonus- only b part is completed.)
The working is documented here.

# LIMITATIONS OF SIMPLE SHELL
# 1. cd command 
cd would not work, this is because cd requires changing directory, and this simple shell is made with the help of fork(). So, if we change directory of child process using execv, the parent would still be in the previous directory.
So, cd cannot be implemented.

# 2. ls ~
Shell specific expansions or features such as ~
ls along with any directory name works fine, but ls ~ would not work since ~ is shell specific and not an external command, so ~ is recognized only by the shell and not by exec family.

# 3. $HOME, $PATH etc.
These are only defined within the shell.
Similarly, Functions defined within the shell or variables set within the shell won't be available in this implementation.

4. MAX number of characters, as defined in the code for a command can be upto 1000 characters. Since, no limit on command size is given taking even larger commands may lead to huge overhead.

------------------------------------------------------------------

NOW,
The starter code such as for shell loop etc. has been created with the help of lecture slides and man pages.

EXECVP is used (member of exec family), and errors checking is done wherever necessary.

# SIMPLE SHELL IMPLEMENTATION
The main function defines a bunch of global variables to be used later, and defines a sigaction handler to change the action for CTRL+C interrupt, so as to display process details when terminating.
Main function also contains the shell loop.
Inside the shell loop there is a launch function with executes command and returns status. Whenever status is 1 (due to failure), loop exits.
Also, in shell loop if input is history, we display history, which is done by storing commands executed in a global array.
Similary, each time a command is executed, its details such as pid, time etc. are taken and returned in global arrays, so that we can display them at time of execution.
Now, to launch a command, user input is taken.
If input is for an .sh file, the shell is able to execute all the commands in it.
Otherwise, normal execution is followed.

For execution, we take the input and parse it first on the basis of delimiter "|" pipe. 
If there is no pipe, the input is parsed on basis of " " spaces. Then we call fork and provide the array of arguments to the execvp call and process gets created.
then in parent, we wait for it to terminate, note the pid and return 0 on success.
If piping is involved, we take each command (Separated by |), process it as individual command, by steps mentioned above, and then reroute the output to the next command.
This is done by pipes(). We create an array of pipes according to number of pipes, now each command has a pipe() with read and write end.
We just use dup2() to duplicate stdin with read, and stdout with write end of pipeline to allow transfer.
So, each command just reads (that is Takes in the input) from the previous pipe, and writes to the current pipe. IN this way, | is implemented.
And all this occurs in a for loop for number of commands, and execvp still is used in child by fork() call.

After this, we can show history at any point in between with help of global arrays where we have stored data.

Now, to terminate and display details, we have caught the signal SIGINT and changed its action, by creating a personal handler and printing the details in the handler method and exiting the process when CTRL C is generated.

# CONTRIBUTION
1. Kirti Jain, 2022250     -- Equal Contribution (Simple Shell + Error Handling)
2. Tanish Verma, 2022532   -- Equal Contribution (Simple Shell + Error Handling)

Both contributed equally.

# Link to private repo:
https://github.com/KiwiiKirtii/OS_Assignment2