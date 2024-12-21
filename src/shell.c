#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT_SIZE 1024

// Function to create a new terminal and run a shell program

void shell_loop(){
    
}
void create_new_terminal() {
    pid_t pid = fork();
    if (pid < 0) {
        // Fork failed
        perror("Failed to create new process");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: create a new session
        if (setsid() < 0) {
            perror("Failed to create a new session");
            exit(EXIT_FAILURE);
        }

        // Run the shell program in a new terminal session
        execlp("wt.exe", "wt.exe", "new-tab", "wsl.exe", "-e", "./bin/myshell", (char*)NULL);

        // If execvp returns, there was an error
        perror("Error executing shell program");
        exit(EXIT_FAILURE);
    } else {
        // Parent process does not need to wait for the child in detached mode
        printf("New terminal launched to run the shell.\n");
    }
}

int main() {
    // Automatically open a new terminal and run the shell
    create_new_terminal();
    return 0;
}
