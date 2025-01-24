#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 100


void execute_help(char* command) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Failed to create child process");
        return;
    } else if (pid == 0) {
        // Child process
        char* bash_args[] = {"/bin/bash", "-c", command, NULL};
        execvp("/bin/bash", bash_args);
        perror("Error executing command");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

void execute_command(char** args) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("failed to create child");
        return;
    } else if (pid == 0) {
        close(STDOUT_FILENO); // Closes standard output (stdout).
        open("./p4.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);

        if (execvp(args[0], args) == -1) {
            perror("command did not execute");
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

void change_directory(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument after cd");
    } else {
        if (chdir(args[1]) != 0) {
            perror("error changing directory");
        }
    }
}

void parse_command(char* command) {
    char* args[MAX_ARGS];
    int i = 0;
    char* token = strtok(command, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
    if (args[0] != NULL) {
        if (strcmp(args[0], "cd") == 0) {
            change_directory(args);
        } else if (strcmp(args[0], "help") == 0) {
        execute_help("help");
        return;
    } else {
            execute_command(args);
        }
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    char cwd[1024];
    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("@LaxmanGupta(Myshell):%s >> ", cwd);
        } else {
            perror("getcwd() error");
            return 1;
        }
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nthanks using my shell\n");
            break;
        }
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        if (strcmp(input, "exit") == 0) {
            printf("Exiting shell...\n");
            break;
        }
        parse_command(input);
    }

    return 0;
}
