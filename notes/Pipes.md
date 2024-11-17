### Notes on Pipes in C

Pipes are a fundamental interprocess communication (IPC) mechanism in Unix-based operating systems. They allow data to be transferred between processes in a unidirectional or bidirectional manner.

---

### 1. **What is a Pipe?**
- A pipe is a communication channel between processes.
- It operates on the producer-consumer principle: one process writes data into the pipe (producer), and another reads from it (consumer).
- Pipes are represented as a pair of file descriptors:
  - **Write-end**: For writing data into the pipe.
  - **Read-end**: For reading data from the pipe.

---

### 2. **Types of Pipes**
1. **Anonymous Pipes**:
   - Used for communication between related processes (parent-child or siblings).
   - Created using the `pipe()` system call.
   - Limited to unidirectional communication.

2. **Named Pipes (FIFOs)**:
   - Used for communication between unrelated processes.
   - Created using the `mkfifo()` system call.
   - Exist as special files in the filesystem and support bidirectional communication.

---

### 3. **Creating and Using Pipes**

#### **Anonymous Pipes**
1. **System Call: `pipe()`**
   - Syntax: 
     ```c
     int pipe(int pipefd[2]);
     ```
     - `pipefd[0]`: File descriptor for the read-end.
     - `pipefd[1]`: File descriptor for the write-end.
   - Returns `0` on success, `-1` on error.

2. **Example: Parent-Child Communication**
   ```c
   #include <stdio.h>
   #include <unistd.h>
   #include <string.h>
   
   int main() {
       int pipefd[2];
       pid_t pid;
       char write_msg[] = "Hello, child!";
       char read_msg[50];
       
       if (pipe(pipefd) == -1) {
           perror("pipe failed");
           return 1;
       }
       
       pid = fork();
       if (pid < 0) {
           perror("fork failed");
           return 1;
       }
       
       if (pid == 0) { // Child process
           close(pipefd[1]); // Close write-end
           read(pipefd[0], read_msg, sizeof(read_msg));
           printf("Child received: %s\n", read_msg);
           close(pipefd[0]);
       } else { // Parent process
           close(pipefd[0]); // Close read-end
           write(pipefd[1], write_msg, strlen(write_msg) + 1);
           close(pipefd[1]);
       }
       return 0;
   }
   ```

---

#### **Named Pipes (FIFOs)**
1. **System Call: `mkfifo()`**
   - Syntax: 
     ```c
     int mkfifo(const char *pathname, mode_t mode);
     ```
   - `pathname`: Name of the FIFO file.
   - `mode`: Permissions for the FIFO (e.g., `0666` for read and write access).

2. **Example: Two Unrelated Processes Communication**
   - **Process 1: Writer**
     ```c
     #include <stdio.h>
     #include <fcntl.h>
     #include <unistd.h>
     #include <string.h>
     
     int main() {
         const char *fifo_path = "/tmp/my_fifo";
         mkfifo(fifo_path, 0666);
         
         int fd = open(fifo_path, O_WRONLY);
         char message[] = "Hello from writer!";
         write(fd, message, strlen(message) + 1);
         close(fd);
         return 0;
     }
     ```

   - **Process 2: Reader**
     ```c
     #include <stdio.h>
     #include <fcntl.h>
     #include <unistd.h>
     
     int main() {
         const char *fifo_path = "/tmp/my_fifo";
         char buffer[50];
         
         int fd = open(fifo_path, O_RDONLY);
         read(fd, buffer, sizeof(buffer));
         printf("Reader received: %s\n", buffer);
         close(fd);
         return 0;
     }
     ```

---

### 4. **Key Characteristics of Pipes**
1. **Unidirectional** (Anonymous Pipes):
   - Data flows from write-end to read-end.
   - For bidirectional communication, you need two pipes.

2. **Blocking Behavior**:
   - **Reading**: Blocks if there’s no data in the pipe.
   - **Writing**: Blocks if the pipe buffer is full.

3. **Buffering**:
   - Pipes have a limited kernel buffer size (e.g., 64KB on Linux).
   - Writing beyond the buffer size blocks the writing process until space becomes available.

4. **File Descriptor Sharing**:
   - Pipes work by sharing file descriptors between processes.

---

### 5. **Error Handling**
- Common errors in pipe usage:
  1. **`EFAULT`**: Invalid address for `pipefd`.
  2. **`EMFILE`**: Too many file descriptors are open.
  3. **`EPIPE`**: Writing to a pipe with no reader causes the process to receive a `SIGPIPE`.

---

### 6. **Advanced Topics**
1. **Full-Duplex Communication**:
   - Use two pipes for bidirectional communication:
     ```c
     int pipe1[2], pipe2[2];
     pipe(pipe1); // Parent -> Child
     pipe(pipe2); // Child -> Parent
     ```

2. **Non-Blocking I/O**:
   - Use `fcntl()` to set the pipe's file descriptors to non-blocking mode:
     ```c
     fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
     ```

3. **Pipe with `select()` or `poll()`**:
   - Monitor multiple file descriptors simultaneously to implement more complex communication.

---

### 7. **Limitations of Pipes**
- Anonymous pipes are only suitable for parent-child communication.
- Limited buffer size can cause blocking under heavy loads.
- Not suitable for large-scale or network-based communication (use sockets for such cases).

---

### 8. **Debugging Pipes**
- Use tools like `strace` to monitor system calls for pipes:
  ```bash
  strace ./pipe_program
  ```
- Check the pipe buffer size:
  ```bash
  cat /proc/sys/fs/pipe-max-size
  ```

---

### Summary
Pipes are a lightweight and efficient mechanism for IPC in Unix systems. They are best suited for communication between processes that share a parent-child relationship or need simple data transfer. For more complex communication needs, other IPC mechanisms like shared memory or message queues might be more appropriate.