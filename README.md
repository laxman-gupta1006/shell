# Build Your Own Shell - Hint Guide

This guide provides hints for implementing a basic Unix shell from scratch. Each section contains guiding questions and tips without direct solutions, encouraging you to think through the implementation.

## Prerequisites
- What C system calls might you need for process management?
- Which header files typically contain these syscalls?
- How comfortable are you with string manipulation in C?

## Core Components to Consider

### 1. Command Line Parsing
- How will you handle whitespace between arguments?
- What's a good data structure to store command arguments?
- Should you handle quotes? How might they affect parsing?
- Consider: What happens when a user presses the up arrow in bash?

### 2. Process Creation
- Which syscall creates new processes?
- How does a child process differ from its parent?
- What happens to file descriptors during forking?
- Think about: How does the shell know when a child process ends?

### 3. Built-in Commands
- Why implement some commands as built-ins?
- Minimum built-ins to consider:
  - `cd`: Why can't this be an external program?
  - `exit`: What state needs cleaning before exit?
  - `help`: What information should users see?

### 4. Path Resolution
- Where does the shell look for executables?
- How to handle relative vs absolute paths?
- What environment variable is crucial here?

### 5. Signal Handling
- What happens when user presses Ctrl+C?
- Should it affect the shell, the running program, or both?
- How to handle background processes?

### 6. Input/Output Redirection
- What do `<`, `>`, and `>>` do in a shell?
- Which file descriptors are involved?
- When should redirection happen - parent or child process?

### 7. Pipes
- How does `ls | grep foo` work internally?
- What connects the output of one process to input of another?
- Think about: How many processes are created for `a | b | c`?

### 8. Error Handling
- What should happen if a command isn't found?
- How to handle fork/exec failures?
- Should errors go to stderr or stdout?

## Advanced Features to Consider

### Environment Variables
- How does the shell store and access environment variables?
- What happens when you type `$PATH`?
- Think about: How is `export` different from regular assignment?

### Job Control
- What makes `&` work for background processes?
- How might you implement `fg` and `bg`?
- Consider: How does shell keep track of stopped jobs?

## Testing Strategy
- How will you test interactive features?
- What edge cases should you consider?
- Think about: How to handle malformed input?

## Common Pitfalls
- Memory leaks in long-running programs
- Zombie processes
- Race conditions with signals
- Buffer overflows in input handling

## Resources to Explore
- POSIX standard documentation
- Man pages: `exec`, `fork`, `wait`, `pipe`, `dup2`
- Source code of existing shells

## Suggested Implementation Order
1. Basic command execution
2. Command line parsing
3. Built-in commands
4. Path resolution
5. Signal handling
6. Redirection
7. Pipes
8. Job control

Remember: A shell is a tool that helps users interact with the operating system. Each feature you add should make this interaction more efficient or powerful.

---

Start simple, test thoroughly, and gradually add complexity. Good luck!