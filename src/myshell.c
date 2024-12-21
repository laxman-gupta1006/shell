#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT_SIZE 1024

void handling_pipes(char* command){

}

void execute_command(char* command) {
    pid_t pid = fork();
    if (pid < 0) {
        // Fork failed
        perror("Failed to create child process");
        return;
    } else if (pid == 0) {
        // Child process
        char* args[100];
        char* com[200];
        int i = 0,j=0;
        char* pipe=strtok(command,"|");
        while(pipe != NULL){
            com[j++]=pipe;
            pipe=strtok(NULL,"|");
        }
        char* token = strtok(command, " ");
        while (token != NULL) {
            args[i++] = token;
            printf("%s\n",token);
            if(strcmp(token," ") == 0){
                printf("Pipe found");
            }
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        execvp(args[0], args);
        perror("Error executing command");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to complete
    }
}

int main() {
    char input[MAX_INPUT_SIZE];

    while (1) {
        printf("@LaxmanGupta >> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Thanks for using my shell\n");
            break;
        }

        // Remove trailing newline character if present
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Execute the command
        execute_command(input);
    }

    return 0;
}