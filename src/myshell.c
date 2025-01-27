#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 100

void run_help(char* cmd) {
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("child creation failed");
        return;
    } else if (child_pid == 0) {
        char* bash_args[] = {"/bin/bash", "-c", cmd, NULL};
        execvp("/bin/bash", bash_args);
        perror("error executing command");
        exit(EXIT_FAILURE);
    } else {
        int child_status;
        waitpid(child_pid, &child_status, 0);
    }
}

void run_command(char *cmd_args[], char *in_file, char *out_file) {
    pid_t child_pid;
    int in_fd, out_fd;
    child_pid = fork();
    if (child_pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        if (in_file != NULL) {
            in_fd = open(in_file, O_RDONLY);
            if (in_fd < 0) {
                perror("unable to open input file");
                exit(EXIT_FAILURE);
            }
            close(STDIN_FILENO);   
            if (open(in_file, O_RDONLY) != STDIN_FILENO) {
                perror("failed to redirect input");
                exit(EXIT_FAILURE);
            }
            close(in_fd);
        }
        if (out_file != NULL) {
            out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) {
                perror("faild to open output file");
                exit(EXIT_FAILURE);
            }
            close(STDOUT_FILENO);       
            if (open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644) != STDOUT_FILENO) { 
                perror("redirection failed");
                exit(EXIT_FAILURE);
            }
            close(out_fd);
        }
        execvp(cmd_args[0], cmd_args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL); 
    }
}

void change_dir(char** cmd_args) {
    if (cmd_args[1] == NULL) {
        fprintf(stderr, "expected argument after cd");
    } else {
        if (chdir(cmd_args[1]) != 0) {
            perror("error changing directory");
        }
    }
}

void parse_cmd(char *cmd) {
    char *cmd_args[MAX_ARGS];
    char *in_file=NULL;
    char *out_file=NULL;
    int i=0;
    int j=0;
    char current_arg[MAX_INPUT_SIZE]={0};
    int current_arg_len=0;
    int inside_quotes=0;
    for (int idx=0; cmd[idx]!='\0'; idx++) {
        char c=cmd[idx];
        if (c==' ' && !inside_quotes) {
            if (current_arg_len>0) {
                current_arg[current_arg_len]='\0';
                cmd_args[i++]=strdup(current_arg);
                current_arg_len=0;
            }
        } else if (c=='"') {
            inside_quotes=!inside_quotes;
        } else if (c=='<' || c=='>') {
            if (current_arg_len>0) {
                current_arg[current_arg_len]='\0';
                cmd_args[i++]=strdup(current_arg);
                current_arg_len=0;
            }
            while (cmd[++idx]==' ') {}
            char file_name[MAX_INPUT_SIZE]={0};
            int file_idx=0;
            while (cmd[idx]!='\0' && cmd[idx]!=' ') {
                file_name[file_idx++]=cmd[idx++];
            }
            file_name[file_idx]='\0';
            if (c=='<') {
                in_file=strdup(file_name);
            } else if (c=='>') {
                out_file=strdup(file_name);
            }
            idx--;
        } else {
            current_arg[current_arg_len++]=c;
        }
    }

    if (current_arg_len>0) {
        current_arg[current_arg_len]='\0';
        cmd_args[i++]=strdup(current_arg);
    }

    cmd_args[i]=NULL;

    if (cmd_args[0]!=NULL) {
        if (strcmp(cmd_args[0],"cd")==0) {
            if (chdir(cmd_args[1])!=0) {
                perror("error changing directory");
            } else {
                printf("Changed directory to: %s\n",cmd_args[1]);
            }
        } else {
            run_command(cmd_args,in_file,out_file);
        }
    }
}

int main() {
    char input_buf[MAX_INPUT_SIZE];
    char curr_dir[1024];
    while (1) {
        if (getcwd(curr_dir, sizeof(curr_dir)) != NULL) {
            printf("@LaxmanGupta(Myshell):%s >> ", curr_dir);
        } else {
            perror("getcwd() error");
            return 1;
        }
        fflush(stdout);
        if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
            printf("\nthanks using my shell\n");
            break;
        }
        size_t input_len = strlen(input_buf);
        if (input_len > 0 && input_buf[input_len - 1] == '\n') {
            input_buf[input_len - 1] = '\0';
        }
        if (strcmp(input_buf, "exit") == 0) {
            printf("exiting shell...\n");
            break;
        }
        parse_cmd(input_buf);
    }
    return 0;
}
