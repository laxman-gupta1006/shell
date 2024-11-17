Below is an example of an `.md` file that provides detailed notes about **Pipes** and **Socket Pairs** in C:

---

# Pipes and Sockets in C

## 1. Overview

Interprocess communication (IPC) is essential for enabling processes to exchange data. **Pipes** and **socket pairs** are common IPC mechanisms in C.

---

## 2. Pipes

### 2.1 What is a Pipe?

- A **pipe** is a unidirectional communication channel that connects two processes.
- Data written to the pipe's write end can be read from the pipe's read end.

### 2.2 Types of Pipes

1. **Anonymous Pipes**  
   - Used for communication between parent and child processes.
   - Created using the `pipe()` system call.
   - Limited to processes with a common ancestry.

   ```c
   int fd[2];
   if (pipe(fd) == -1) {
       perror("pipe");
       exit(EXIT_FAILURE);
   }
   ```

   - `fd[0]`: Read end of the pipe.
   - `fd[1]`: Write end of the pipe.

2. **Named Pipes (FIFOs)**  
   - Can be used for communication between unrelated processes.
   - Created using the `mkfifo()` function.

   ```c
   if (mkfifo("myfifo", 0666) == -1) {
       perror("mkfifo");
       exit(EXIT_FAILURE);
   }
   ```

### 2.3 Operations

| Operation | Description          | System Call   |
|-----------|----------------------|---------------|
| Write     | Write to the pipe.   | `write()`     |
| Read      | Read from the pipe.  | `read()`      |
| Close     | Close an end of pipe.| `close()`     |

---

## 3. Socket Pairs

### 3.1 What is a Socket Pair?

- A **socket pair** is a bidirectional communication mechanism.
- It is implemented using the `socketpair()` system call.

### 3.2 Syntax

```c
int sv[2];
if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
    perror("socketpair");
    exit(EXIT_FAILURE);
}
```

- `sv[0]` and `sv[1]` are both bidirectional endpoints.

### 3.3 Comparison with Pipes

| Feature                | Pipes                   | Socket Pairs         |
|------------------------|-------------------------|----------------------|
| Directionality         | Unidirectional          | Bidirectional        |
| Communication Scope    | Parent-Child Processes  | Parent-Child, Threads|
| Complexity             | Simple                 | Slightly Complex     |

---

## 4. Common Use Cases

| Mechanism    | Use Case                          |
|--------------|-----------------------------------|
| Anonymous Pipe | Parent-to-Child Data Transfer   |
| Named Pipe   | Communication between Apps       |
| Socket Pair  | Bidirectional Thread Messaging   |

---

## 5. Example Programs

### 5.1 Anonymous Pipe Example

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int fd[2];
    char buffer[128];
    
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        close(fd[0]);  // Close read end
        write(fd[1], "Hello from child!", 17);
        close(fd[1]);  // Close write end
    } else {
        close(fd[1]);  // Close write end
        read(fd[0], buffer, sizeof(buffer));
        printf("Parent received: %s\n", buffer);
        close(fd[0]);  // Close read end
    }

    return 0;
}
```

### 5.2 Socket Pair Example

```c
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int sv[2];
    char buffer[128];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        perror("socketpair");
        exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
        close(sv[0]);  // Close one end
        write(sv[1], "Message from child!", 19);
        close(sv[1]);  // Close after use
    } else {
        close(sv[1]);  // Close one end
        read(sv[0], buffer, sizeof(buffer));
        printf("Parent received: %s\n", buffer);
        close(sv[0]);  // Close after use
    }

    return 0;
}
```

---

## 6. Best Practices

- Always close unused ends of pipes or sockets.
- Use non-blocking IO or `select()` for advanced control.
- Handle errors gracefully with proper checks for system calls.

---

## 7. Additional Resources

- `man pipe`
- `man socketpair`
- [Linux IPC Documentation](https://man7.org/linux/man-pages/)

---

This file provides a comprehensive summary of pipes and socket pairs in C. Adjust based on your requirements or audience!