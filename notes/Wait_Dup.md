### Notes on `wait()` and `dup()` in C

---

### 1. **`wait()` System Call**

The `wait()` family of system calls is used to synchronize a parent process with its child processes. It allows the parent to wait for the termination of a child process and retrieve its exit status.

---

#### **Syntax**

```c
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
```

- **Arguments**:
  - `status`: Pointer to an integer where the child's exit status is stored.
  - `pid` (in `waitpid`): Specifies the child process to wait for.
  - `options`: Specifies additional options (e.g., `WNOHANG`, `WUNTRACED`).

- **Return Value**:
  - Returns the PID of the terminated child on success.
  - Returns `-1` on error and sets `errno`.

---

#### **Key Features**

1. **Parent-Child Synchronization**:
   - Ensures the parent process can wait for the child to complete its execution.

2. **Exit Status**:
   - `WIFEXITED(status)`: Returns `true` if the child terminated normally.
   - `WEXITSTATUS(status)`: Retrieves the child's exit code.
   - `WIFSIGNALED(status)`: Returns `true` if the child terminated due to a signal.

3. **Zombie Processes**:
   - A terminated child becomes a zombie process until the parent calls `wait()`.

---

#### **Basic Example**

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) { // Child process
        printf("Child process running\n");
        return 42; // Exit with status 42
    } else { // Parent process
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}
```

---

#### **Using `waitpid()`**

- `waitpid()` allows waiting for specific child processes or provides non-blocking behavior.

```c
pid_t waitpid(pid_t pid, int *status, int options);
```

- **Options**:
  - `WNOHANG`: Non-blocking wait.
  - `WUNTRACED`: Report stopped child processes.

**Example**:
```c
pid_t pid = fork();

if (pid == 0) {
    // Child process
    sleep(2);
    return 0;
} else {
    // Parent process
    int status;
    while (waitpid(pid, &status, WNOHANG) == 0) {
        printf("Child still running...\n");
        sleep(1);
    }
    printf("Child exited\n");
}
```

---

#### **Common Pitfalls**

1. **Zombie Processes**:
   - Always call `wait()` to clean up terminated child processes.

2. **Blocking Behavior**:
   - Without `WNOHANG`, `wait()` and `waitpid()` block the parent process until the child exits.

3. **Multiple Children**:
   - Use loops to `wait()` for multiple children.

---

---

### 2. **`dup()` System Call**

The `dup()` and `dup2()` system calls are used to duplicate file descriptors. They are particularly useful for redirecting standard input/output or managing file descriptors.

---

#### **Syntax**

```c
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);
```

- **Arguments**:
  - `oldfd`: File descriptor to duplicate.
  - `newfd` (in `dup2`): The desired new file descriptor.

- **Return Value**:
  - `dup()`: Returns the new file descriptor or `-1` on error.
  - `dup2()`: Returns `newfd` or `-1` on error.

---

#### **Key Features**

1. **File Descriptor Duplication**:
   - `dup()` creates a copy of `oldfd` using the lowest available file descriptor.
   - `dup2()` allows specifying the exact file descriptor number for the duplicate.

2. **Sharing File Offsets**:
   - The new file descriptor shares the same file offset and flags as the original.

3. **Redirection**:
   - Commonly used to redirect standard input/output to files or other streams.

---

#### **Basic Examples**

**Using `dup()`**:
```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int newfd = dup(fd); // Duplicate fd
    write(newfd, "Hello, dup()\n", 13);
    close(fd);
    close(newfd);
    return 0;
}
```

**Using `dup2()` for Redirection**:
```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, STDOUT_FILENO); // Redirect stdout to file
    printf("This will go to output.txt\n");
    close(fd);
    return 0;
}
```

---

#### **Differences Between `dup()` and `dup2()`**

| Feature             | `dup()`                      | `dup2()`                             |
|---------------------|------------------------------|--------------------------------------|
| File Descriptor     | Uses the lowest available FD | Can specify the FD to duplicate to. |
| Overwriting FDs     | Cannot overwrite existing FDs| Overwrites if `newfd` is open.      |

---

#### **Advanced Usage**

1. **Combining `dup()` with Pipes**:
   - Redirect the output of one process to the input of another.
   - Example with `pipe()`:
     ```c
     int fd[2];
     pipe(fd); // Create a pipe
     if (fork() == 0) { 
         dup2(fd[1], STDOUT_FILENO); // Redirect stdout to pipe write-end
         close(fd[0]);
         execlp("ls", "ls", NULL); // Child runs 'ls'
     } else {
         dup2(fd[0], STDIN_FILENO); // Redirect stdin to pipe read-end
         close(fd[1]);
         execlp("wc", "wc", "-l", NULL); // Parent runs 'wc -l'
     }
     ```

2. **Custom Standard Streams**:
   - Replace `STDOUT_FILENO` or `STDIN_FILENO` for custom input/output redirection.

---

#### **Common Pitfalls**

1. **File Descriptor Leaks**:
   - Always close unused file descriptors after duplication.

2. **Overwriting Behavior**:
   - Be cautious with `dup2()` as it forcibly overwrites `newfd`.

---

### Summary

- **`wait()`**:
  - Used to synchronize parent and child processes.
  - Helps clean up zombie processes and retrieve child exit status.
- **`dup()`**:
  - Duplicates file descriptors for I/O redirection or sharing.
  - Useful in inter-process communication and resource redirection.