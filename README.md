# MyShell - Custom Unix Shell Implementation

A basic Unix shell implementation written in C that supports core shell functionality including command execution, built-in commands, and I/O redirection.

## Features Implemented

### Core Shell Functionality
- **Interactive Command Prompt**: Displays current working directory in the format `@LaxmanGupta(Myshell):<current_directory> >>`
- **Command Line Parsing**: Handles whitespace, arguments, and quoted strings
- **Process Creation**: Uses `fork()` and `execvp()` for command execution
- **Process Management**: Proper parent-child process handling with `wait()`

### Built-in Commands
- **`cd <directory>`**: Change current working directory
  - Handles directory change errors with appropriate error messages
- **`exit`**: Gracefully exits the shell with a goodbye message
- **`jobs`**: List all active background processes

### Input/Output Redirection
- **Input Redirection (`<`)**: Redirect stdin from a file
  - Example: `cat < input.txt`
- **Output Redirection (`>`)**: Redirect stdout to a file (overwrites)
  - Example: `ls > output.txt`
- Proper file descriptor management and error handling

### Pipe Support
- **Command Pipelines (`|`)**: Chain multiple commands together
  - Example: `ls -la | grep .txt | wc -l`
- **Multi-stage Pipelines**: Support for multiple pipes in a single command
  - Example: `cat file.txt | grep pattern | sort | uniq`
- Proper inter-process communication using Unix pipes

### Background Process Support
- **Background Execution (`&`)**: Run commands in the background
  - Example: `sleep 10 &`
- **Job Control**: Track and manage background processes
- **Job Listing**: Use `jobs` command to see active background processes
- Automatic notification when background jobs complete

### Signal Handling
- **Ctrl+C (SIGINT)**: Interrupt foreground processes without killing the shell
- **Ctrl+Z (SIGTSTP)**: Suspend foreground processes
- **Child Process Management**: Automatic cleanup of zombie processes
- Background processes ignore interrupt signals

### Command Parsing Features
- **Quote Handling**: Supports quoted arguments with spaces
- **File Redirection Parsing**: Automatically detects and handles `<` and `>`
- **Argument Tokenization**: Splits commands into proper argument arrays

### Error Handling
- Fork failure detection
- File opening errors for redirection
- Directory change errors
- Command execution failures
- Input validation and edge case handling

## Technical Implementation

### System Calls Used
- `fork()` - Process creation
- `execvp()` - Program execution
- `wait()` / `waitpid()` - Process synchronization
- `chdir()` - Directory changes
- `open()`, `close()` - File operations
- `getcwd()` - Current directory retrieval
- `pipe()` - Inter-process communication
- `dup2()` - File descriptor duplication
- `signal()` - Signal handling
- `kill()` - Send signals to processes

### Memory Management
- Dynamic memory allocation with `strdup()`
- Proper string handling and null termination
- Buffer overflow protection

## Build Instructions

### Compile the Shell
```bash
make shell
```

### Compile Optimized Version
```bash
make shell_optimized
```

### Run the Shell
```bash
make run
```

## Usage Examples

### Basic Commands
```bash
@LaxmanGupta(Myshell):/home/user >> ls
@LaxmanGupta(Myshell):/home/user >> pwd
@LaxmanGupta(Myshell):/home/user >> date
```

### Built-in Commands
```bash
@LaxmanGupta(Myshell):/home/user >> cd Documents
@LaxmanGupta(Myshell):/home/user/Documents >> jobs
@LaxmanGupta(Myshell):/home/user/Documents >> exit
```

### I/O Redirection
```bash
@LaxmanGupta(Myshell):/home/user >> ls > filelist.txt
@LaxmanGupta(Myshell):/home/user >> cat < input.txt
@LaxmanGupta(Myshell):/home/user >> grep "pattern" < data.txt > results.txt
```

### Pipes
```bash
@LaxmanGupta(Myshell):/home/user >> ls -la | grep .txt
@LaxmanGupta(Myshell):/home/user >> cat file.txt | grep pattern | sort | uniq
@LaxmanGupta(Myshell):/home/user >> ps aux | grep shell
```

### Background Processes
```bash
@LaxmanGupta(Myshell):/home/user >> sleep 10 &
[1] 12345
@LaxmanGupta(Myshell):/home/user >> jobs
Active background jobs:
[1] 12345    sleep 10 &
@LaxmanGupta(Myshell):/home/user >> 
[1] Done    sleep 10 &
```

### Quoted Arguments
```bash
@LaxmanGupta(Myshell):/home/user >> echo "Hello World"
@LaxmanGupta(Myshell):/home/user >> mkdir "My Documents"
```

## Project Structure
```
shell/
├── src/
│   └── shell.c          # Main shell implementation
├── bin/                 # Compiled binaries (created after build)
├── notes/              # Documentation and implementation notes
│   ├── Pipes.md
│   ├── Fork.md
│   ├── Exec.md
│   ├── Wait_Dup.md
│   ├── Signal.md
│   └── Makefile.md
├── Makefile            # Build configuration
└── README.md           # This file
```

## Current Limitations

### Not Yet Implemented
- Environment variable expansion (`$VAR`)
- Command history
- Tab completion
- Append redirection (`>>`)
- Advanced job control (`fg`, `bg` commands)

## Notes and Documentation

This repository includes detailed implementation notes:

- [About Pipes](./notes/Pipes.md)
- [About Fork](./notes/Fork.md)
- [About Exec](./notes/Exec.md)
- [About Wait and Dup](./notes/Wait_Dup.md)
- [Signals in C](./notes/Signal.md)
- [Makefile Guide](./notes/Makefile.md)

## Author
Laxman Gupta

---

This shell demonstrates fundamental Unix system programming concepts including process management, file I/O, and system call usage in C.