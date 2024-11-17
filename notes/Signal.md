### Notes on Signals in C

Signals are a form of inter-process communication in Unix-like operating systems, allowing processes to send asynchronous notifications to each other or handle specific events. Signals are used to indicate various conditions such as program errors, external interrupts, or user-defined events.

---

### 1. **What Are Signals?**
- Signals are software interrupts delivered to a process to notify it of an event.
- Each signal is identified by a unique integer constant (e.g., `SIGINT`, `SIGKILL`).
- Processes can:
  1. Ignore the signal.
  2. Handle the signal with a custom handler.
  3. Allow the default action.

---

### 2. **Common Signals**

| Signal      | Value | Default Action          | Description                         |
|-------------|-------|-------------------------|-------------------------------------|
| `SIGINT`    | 2     | Terminate               | Sent when user presses Ctrl+C.     |
| `SIGKILL`   | 9     | Terminate               | Forcefully terminates a process.   |
| `SIGTERM`   | 15    | Terminate               | Requests process termination.      |
| `SIGSEGV`   | 11    | Terminate (core dump)   | Invalid memory access (segfault).  |
| `SIGALRM`   | 14    | Terminate               | Sent after a timer expires.        |
| `SIGCHLD`   | 17    | Ignore                  | Sent to parent when child exits.   |
| `SIGHUP`    | 1     | Terminate               | Hangup detected on terminal.       |
| `SIGUSR1`   | 10    | Terminate               | User-defined signal 1.             |
| `SIGUSR2`   | 12    | Terminate               | User-defined signal 2.             |

---

### 3. **Signal Handling in C**

#### **Header Files**
```c
#include <signal.h>
#include <unistd.h>
```

---

### 4. **Default Signal Behavior**
- Signals have predefined default actions:
  - **Terminate**: Ends the process (e.g., `SIGKILL`, `SIGTERM`).
  - **Ignore**: Ignores the signal (e.g., `SIGCHLD`).
  - **Core Dump**: Terminates the process and creates a core dump (e.g., `SIGSEGV`).
  - **Stop/Continue**: Stops or resumes the process (e.g., `SIGSTOP`, `SIGCONT`).

---

### 5. **Custom Signal Handlers**

A signal handler is a function that specifies custom behavior when a signal is received.

#### Example: Setting a Signal Handler
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_signal(int sig) {
    printf("Caught signal %d\n", sig);
}

int main() {
    signal(SIGINT, handle_signal); // Set custom handler for SIGINT
    while (1) {
        printf("Running... Press Ctrl+C to send SIGINT\n");
        sleep(1);
    }
    return 0;
}
```

#### Key Functions for Signal Handling

1. **`signal()`**:
   - Simplified interface to set signal handlers.
   - Syntax:
     ```c
     void (*signal(int signum, void (*handler)(int)))(int);
     ```
   - `handler` can be:
     - A custom function.
     - `SIG_IGN` (ignore the signal).
     - `SIG_DFL` (use default behavior).

2. **`sigaction()`**:
   - A more robust alternative to `signal()`.
   - Allows detailed control over signal handling.
   - Syntax:
     ```c
     int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
     ```
   - Example:
     ```c
     struct sigaction sa;
     sa.sa_handler = handle_signal; // Set custom handler
     sa.sa_flags = 0;              // No flags
     sigaction(SIGINT, &sa, NULL); // Apply handler for SIGINT
     ```

---

### 6. **Sending Signals**

#### **`kill()`**
Used to send a signal to a specific process.
```c
#include <signal.h>
int kill(pid_t pid, int sig);
```
- `pid`: Process ID to which the signal is sent.
- `sig`: Signal to send.

Example:
```c
kill(getpid(), SIGUSR1); // Send SIGUSR1 to the current process
```

#### **`raise()`**
Sends a signal to the current process.
```c
#include <signal.h>
int raise(int sig);
```

Example:
```c
raise(SIGUSR1); // Send SIGUSR1 to self
```

---

### 7. **Blocking and Unblocking Signals**

#### **`sigprocmask()`**
Used to block or unblock signals.
```c
#include <signal.h>
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```
- `how`:
  - `SIG_BLOCK`: Block specified signals.
  - `SIG_UNBLOCK`: Unblock specified signals.
  - `SIG_SETMASK`: Replace current mask with the new set.

#### Example
```c
sigset_t set;
sigemptyset(&set);         // Initialize an empty set
sigaddset(&set, SIGINT);   // Add SIGINT to the set
sigprocmask(SIG_BLOCK, &set, NULL); // Block SIGINT
```

---

### 8. **Pausing for Signals**

#### **`pause()`**
Suspends the process until a signal is received.
```c
#include <unistd.h>
int pause(void);
```

Example:
```c
while (1) {
    printf("Waiting for signal...\n");
    pause();
}
```

---

### 9. **Handling Multiple Signals**

#### **`sigset_t` and `sigpending()`**
- `sigset_t`: A data structure for signal sets.
- `sigpending()`: Checks for pending signals.

Example:
```c
sigset_t pending;
sigpending(&pending);
if (sigismember(&pending, SIGINT)) {
    printf("SIGINT is pending\n");
}
```

---

### 10. **Predefined Signal Macros**

| Macro       | Description                         |
|-------------|-------------------------------------|
| `SIG_DFL`   | Default signal handling behavior.   |
| `SIG_IGN`   | Ignore the signal.                 |

---

### 11. **Practical Examples**

#### **Example 1: Ignoring Signals**
```c
signal(SIGINT, SIG_IGN); // Ignore Ctrl+C
```

#### **Example 2: Graceful Shutdown**
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_signal(int sig) {
    printf("Terminating gracefully...\n");
    exit(0);
}

int main() {
    signal(SIGTERM, handle_signal); // Handle SIGTERM for cleanup
    while (1) {
        printf("Running...\n");
        sleep(1);
    }
}
```

#### **Example 3: Handling `SIGCHLD`**
```c
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        printf("Cleaned up a child process\n");
    }
}

int main() {
    signal(SIGCHLD, handle_sigchld);
    if (fork() == 0) {
        printf("Child process\n");
        return 0;
    }
    sleep(5);
    return 0;
}
```

---

### 12. **Common Pitfalls**
1. **Unreliable `signal()` Behavior**:
   - Older implementations may reset the signal handler to `SIG_DFL` after handling.
   - Use `sigaction()` for more robust handling.

2. **Blocking Critical Signals**:
   - Blocking signals like `SIGKILL` is not allowed and can lead to issues.

3. **Signal Safety**:
   - Avoid non-reentrant functions (e.g., `printf()`, `malloc()`) inside signal handlers.

---

### Summary
- Signals are a powerful mechanism for inter-process communication and error handling.
- Use `signal()` or `sigaction()` to define custom handlers.
- Signals like `SIGINT` (Ctrl+C) or `SIGCHLD` are commonly used in process management.
- Avoid signal misuse by handling them gracefully and ensuring safe operations in handlers.