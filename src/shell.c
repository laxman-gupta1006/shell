#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 100
#define MAX_PIPES 10
#define MAX_BACKGROUND_JOBS 100

// Background job structure
typedef struct {
    pid_t pid;
    char command[MAX_INPUT_SIZE];
    int job_id;
    int active;
} bg_job_t;

// Global variables for job control
bg_job_t background_jobs[MAX_BACKGROUND_JOBS];
int next_job_id = 1;
int foreground_pid = 0;

// Signal handlers
void sigint_handler(int sig) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);
        printf("\n");
    } else {
        printf("\n");
        char curr_dir[1024];
        if (getcwd(curr_dir, sizeof(curr_dir)) != NULL) {
            printf("@LaxmanGupta(Myshell):%s >> ", curr_dir);
        }
        fflush(stdout);
    }
}

void sigtstp_handler(int sig) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGTSTP);
        printf("\n[Process suspended]\n");
    }
}

void sigchld_handler(int sig) {
    pid_t pid;
    int status;
    
    // Reap all available zombie children
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Find and remove from background jobs
        for (int i = 0; i < MAX_BACKGROUND_JOBS; i++) {
            if (background_jobs[i].active && background_jobs[i].pid == pid) {
                printf("\n[%d] Done    %s\n", background_jobs[i].job_id, background_jobs[i].command);
                background_jobs[i].active = 0;
                break;
            }
        }
    }
}

// Background job management
int add_background_job(pid_t pid, const char* command) {
    for (int i = 0; i < MAX_BACKGROUND_JOBS; i++) {
        if (!background_jobs[i].active) {
            background_jobs[i].pid = pid;
            background_jobs[i].job_id = next_job_id++;
            strcpy(background_jobs[i].command, command);
            background_jobs[i].active = 1;
            return background_jobs[i].job_id;
        }
    }
    return -1;
}

void list_background_jobs() {
    printf("Active background jobs:\n");
    int found = 0;
    for (int i = 0; i < MAX_BACKGROUND_JOBS; i++) {
        if (background_jobs[i].active) {
            printf("[%d] %d    %s\n", background_jobs[i].job_id, background_jobs[i].pid, background_jobs[i].command);
            found = 1;
        }
    }
    if (!found) {
        printf("No active background jobs.\n");
    }
}

// Execute a single command with redirection and background support
void run_command(char *cmd_args[], char *in_file, char *out_file, int background, const char* original_cmd) {
    pid_t child_pid;
    int in_fd, out_fd;
    
    child_pid = fork();
    if (child_pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        // Child process
        if (background) {
            // For background processes, ignore SIGINT and SIGTSTP
            signal(SIGINT, SIG_IGN);
            signal(SIGTSTP, SIG_IGN);
        }
        
        if (in_file != NULL) {
            in_fd = open(in_file, O_RDONLY);
            if (in_fd < 0) {
                perror("unable to open input file");
                exit(EXIT_FAILURE);
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        
        if (out_file != NULL) {
            out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) {
                perror("failed to open output file");
                exit(EXIT_FAILURE);
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        
        execvp(cmd_args[0], cmd_args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        if (background) {
            int job_id = add_background_job(child_pid, original_cmd);
            printf("[%d] %d\n", job_id, child_pid);
        } else {
            foreground_pid = child_pid;
            int status;
            waitpid(child_pid, &status, 0);
            foreground_pid = 0;
        }
    }
}

// Execute a pipeline of commands
void run_pipeline(char *commands[][MAX_ARGS], int num_commands, char *in_file, char *out_file, int background, const char* original_cmd) {
    int pipes[MAX_PIPES][2];
    pid_t pids[MAX_PIPES + 1];
    
    // Create pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            return;
        }
    }
    
    // Create processes for each command in the pipeline
    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("fork failed");
            return;
        } else if (pids[i] == 0) {
            // Child process
            if (background) {
                signal(SIGINT, SIG_IGN);
                signal(SIGTSTP, SIG_IGN);
            }
            
            // Set up input
            if (i == 0 && in_file != NULL) {
                // First command with input redirection
                int in_fd = open(in_file, O_RDONLY);
                if (in_fd < 0) {
                    perror("unable to open input file");
                    exit(EXIT_FAILURE);
                }
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            } else if (i > 0) {
                // Not the first command, read from previous pipe
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            // Set up output
            if (i == num_commands - 1 && out_file != NULL) {
                // Last command with output redirection
                int out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (out_fd < 0) {
                    perror("failed to open output file");
                    exit(EXIT_FAILURE);
                }
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            } else if (i < num_commands - 1) {
                // Not the last command, write to next pipe
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            // Close all pipe file descriptors in child
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            execvp(commands[i][0], commands[i]);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    }
    
    // Parent process: close all pipe file descriptors
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Wait for all processes
    if (background) {
        int job_id = add_background_job(pids[0], original_cmd);  // Use first PID as representative
        printf("[%d] %d\n", job_id, pids[0]);
    } else {
        foreground_pid = pids[0];  // Set the first process as foreground for signal handling
        for (int i = 0; i < num_commands; i++) {
            int status;
            waitpid(pids[i], &status, 0);
        }
        foreground_pid = 0;
    }
}

void parse_cmd(char *cmd) {
    char *original_cmd = strdup(cmd);  // Save original command for background job tracking
    
    // Check for background process indicator
    int background = 0;
    int cmd_len = strlen(cmd);
    if (cmd_len > 0 && cmd[cmd_len - 1] == '&') {
        background = 1;
        cmd[cmd_len - 1] = '\0';  // Remove the '&'
        // Remove trailing whitespace
        while (cmd_len > 1 && cmd[cmd_len - 2] == ' ') {
            cmd[cmd_len - 2] = '\0';
            cmd_len--;
        }
    }
    
    // Split command by pipes
    char *pipe_commands[MAX_PIPES];
    int num_pipes = 0;
    char *pipe_token = strtok(cmd, "|");
    
    while (pipe_token != NULL && num_pipes < MAX_PIPES) {
        // Trim leading and trailing whitespace
        while (*pipe_token == ' ') pipe_token++;
        char *end = pipe_token + strlen(pipe_token) - 1;
        while (end > pipe_token && *end == ' ') *end-- = '\0';
        
        pipe_commands[num_pipes] = pipe_token;
        num_pipes++;
        pipe_token = strtok(NULL, "|");
    }
    
    if (num_pipes == 0) {
        free(original_cmd);
        return;
    }
    
    // Parse each command in the pipeline
    char *commands[MAX_PIPES][MAX_ARGS];
    char *global_in_file = NULL;
    char *global_out_file = NULL;
    
    for (int pipe_idx = 0; pipe_idx < num_pipes; pipe_idx++) {
        char *current_cmd = pipe_commands[pipe_idx];
        char *cmd_args[MAX_ARGS];
        char *in_file = NULL;
        char *out_file = NULL;
        int i = 0;
        
        char current_arg[MAX_INPUT_SIZE] = {0};
        int current_arg_len = 0;
        int inside_quotes = 0;
        
        for (int idx = 0; current_cmd[idx] != '\0'; idx++) {
            char c = current_cmd[idx];
            
            if (c == ' ' && !inside_quotes) {
                if (current_arg_len > 0) {
                    current_arg[current_arg_len] = '\0';
                    cmd_args[i++] = strdup(current_arg);
                    current_arg_len = 0;
                }
            } else if (c == '"') {
                inside_quotes = !inside_quotes;
            } else if ((c == '<' || c == '>') && !inside_quotes) {
                if (current_arg_len > 0) {
                    current_arg[current_arg_len] = '\0';
                    cmd_args[i++] = strdup(current_arg);
                    current_arg_len = 0;
                }
                
                // Skip whitespace after redirection operator
                while (current_cmd[++idx] == ' ') {}
                
                char file_name[MAX_INPUT_SIZE] = {0};
                int file_idx = 0;
                
                // Handle quoted filenames
                int file_in_quotes = 0;
                if (current_cmd[idx] == '"') {
                    file_in_quotes = 1;
                    idx++;
                }
                
                while (current_cmd[idx] != '\0' && 
                       ((file_in_quotes && current_cmd[idx] != '"') || 
                        (!file_in_quotes && current_cmd[idx] != ' '))) {
                    file_name[file_idx++] = current_cmd[idx++];
                }
                
                if (file_in_quotes && current_cmd[idx] == '"') {
                    idx++;
                }
                
                file_name[file_idx] = '\0';
                
                if (c == '<') {
                    in_file = strdup(file_name);
                    if (pipe_idx == 0) global_in_file = in_file;
                } else if (c == '>') {
                    out_file = strdup(file_name);
                    if (pipe_idx == num_pipes - 1) global_out_file = out_file;
                }
                idx--;
            } else {
                current_arg[current_arg_len++] = c;
            }
        }
        
        if (current_arg_len > 0) {
            current_arg[current_arg_len] = '\0';
            cmd_args[i++] = strdup(current_arg);
        }
        
        cmd_args[i] = NULL;
        
        // Copy arguments to commands array
        for (int j = 0; j < i; j++) {
            commands[pipe_idx][j] = cmd_args[j];
        }
        commands[pipe_idx][i] = NULL;
    }
    
    // Handle built-in commands (only for single commands, not pipelines)
    if (num_pipes == 1 && commands[0][0] != NULL) {
        if (strcmp(commands[0][0], "cd") == 0) {
            if (commands[0][1] == NULL) {
                fprintf(stderr, "cd: expected argument\n");
            } else {
                if (chdir(commands[0][1]) != 0) {
                    perror("cd");
                }
            }
            free(original_cmd);
            return;
        } else if (strcmp(commands[0][0], "jobs") == 0) {
            list_background_jobs();
            free(original_cmd);
            return;
        }
    }
    
    // Execute the command(s)
    if (commands[0][0] != NULL) {
        if (num_pipes == 1) {
            // Single command
            run_command(commands[0], global_in_file, global_out_file, background, original_cmd);
        } else {
            // Pipeline
            run_pipeline(commands, num_pipes, global_in_file, global_out_file, background, original_cmd);
        }
    }
    
    free(original_cmd);
}

int main() {
    char input_buf[MAX_INPUT_SIZE];
    char curr_dir[1024];
    
    // Initialize background jobs array
    for (int i = 0; i < MAX_BACKGROUND_JOBS; i++) {
        background_jobs[i].active = 0;
    }
    
    // Set up signal handlers
    signal(SIGINT, sigint_handler);   // Ctrl+C
    signal(SIGTSTP, sigtstp_handler); // Ctrl+Z
    signal(SIGCHLD, sigchld_handler); // Child process termination
    
    printf("MyShell - Enhanced Unix Shell\n");
    printf("Features: pipes (|), background processes (&), signal handling (Ctrl+C/Ctrl+Z)\n");
    printf("Type 'jobs' to see background processes, 'exit' to quit.\n\n");
    
    while (1) {
        if (getcwd(curr_dir, sizeof(curr_dir)) != NULL) {
            printf("@LaxmanGupta(Myshell):%s >> ", curr_dir);
        } else {
            perror("getcwd() error");
            return 1;
        }
        fflush(stdout);
        
        if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
            printf("\nthanks for using my shell\n");
            break;
        }
        
        size_t input_len = strlen(input_buf);
        if (input_len > 0 && input_buf[input_len - 1] == '\n') {
            input_buf[input_len - 1] = '\0';
        }
        
        // Skip empty commands
        if (strlen(input_buf) == 0) {
            continue;
        }
        
        if (strcmp(input_buf, "exit") == 0) {
            printf("exiting shell...\n");
            break;
        }
        
        parse_cmd(input_buf);
    }
    
    return 0;
}