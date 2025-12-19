# ENSEASH - ENSEA Tiny Shell

## Main Function of Your Code

ENSEASH is a tiny shell implementation written in C that executes commands and displays information about their execution. The shell implements a Read-Eval-Print Loop (REPL) that continuously reads user commands, parses them, and executes them with support for advanced features like pipes, input/output redirection, background job execution, and real-time execution timing.

The core functionality is divided into two main components:
- **enseash.c**: Handles command parsing, execution, and the main shell loop
- **background.c**: Manages background job execution and monitoring

Ps.: Was made one commit per question.

## What Works

### Fully Implemented Features

1. **Welcome Message and Simple Prompt (Question 1)**
   - Displays welcome message on startup: "Welcome to ENSEA Tiny Shell"
   - Shows simple prompt: `enseash %`
   - Proper initialization of the shell environment

2. **REPL Loop - Read, Execute, Print (Question 2)**
   - Reads command entered by user
   - Executes the command (simple commands without arguments initially)
   - Returns to prompt and waits for new command
   - Continuous loop until exit

3. **Exit Command and Ctrl+D Handling (Question 3)**
   - `exit` command terminates the shell with goodbye message
   - Ctrl+D (EOF) also terminates the shell gracefully
   - Proper cleanup before exit

4. **Display Return Code or Signal in Prompt (Question 4)**
   - Shows exit code of previous command: `enseash [exit:0] %`
   - Shows signal number if process was terminated: `enseash [sign:9] %`
   - Uses `WIFEXITED()` and `WIFSIGNALED()` macros for proper status checking
   - Distinguishes between normal exit and signal termination

5. **Execution Time Measurement with clock_gettime (Question 5)**
   - Measures command execution time using `clock_gettime()`
   - Displays timing in milliseconds in prompt: `enseash [exit:0|10ms] %`
   - Works with both fast and slow commands
   - Accurate nanosecond to millisecond conversion

6. **Complex Command Execution with Arguments (Question 6)**
   - Executes commands with multiple arguments
   - Supports command flags and options
   - Proper tokenization using `strtok()` with string copying
   - Examples: `hostname -i`, `fortune -s`

7. **Input/Output Redirection with < and > (Question 7)**
   - Input redirection: `command < input.txt` (stdin from file)
   - Output redirection: `command > output.txt` (stdout to file)
   - Combined redirections: `command < input.txt > output.txt`
   - Proper file descriptor management with `dup2()`
   - Creates files if they don't exist, truncates if they do

8. **Pipe Redirection with | (Question 8)**
   - Single pipes: `ls | wc -l`
   - Multiple pipes: `cat file.txt | grep pattern | wc -l`
   - Pipes with redirections: `ls | wc -l > count.txt`
   - Proper file descriptor management and synchronization
   - Correct pipe creation and closure in parent and child processes

9. **Background Job Execution with & (Question 9)**
   - Background execution with `&`: `sleep 10 &`
   - Returns to prompt immediately: `enseash [1&] %`
   - Job ID assignment and tracking: `[1] 3656`
   - Non-blocking wait with `WNOHANG` flag
   - Notification when background jobs complete: `[1]+ Ended: sleep 10 &`
   - Proper execution time measurement for background jobs
   - Data structure for background process management
   - Display of background job information

## What Doesn't Work?

### Known Limitations

1. **Glob Pattern Expansion**
   - Wildcards like `*.txt`, `?.txt`, `[abc].txt` are not expanded
   - Patterns are passed literally to commands
   - Example: `ls *.txt` fails if no file named literally `*.txt` exists

2. **Advanced Redirection Operators**
   - Append redirection (`>>`) not supported
   - Stderr redirection (`2>`) not supported
   - Combined stdout/stderr (`&>`) not supported

3. **Command History Navigation**
   - No arrow key support for command history
   - Cannot navigate previous commands with ↑/↓



## Learning Outcomes

### What We Learned from This Project

This project taught me the importance of dividing code into well-organized functions with clear responsibilities. Before starting, I didn't fully understand how to structure a shell properly, but I learned that each function should do one thing well. For example, `parse_command()` only handles tokenizing input and extracting arguments, redirections, and background flags. `redirect_fd()` only handles file descriptor redirection with `dup2()`. `execute_pipeline()` only manages creating pipes and forking processes for piped commands. By separating these concerns, the code became much more readable and easier to debug. I also learned that good function names make code self-documenting—when someone reads `split_by_pipe()`, they immediately understand what it does without needing to read the implementation. Working on macOS taught me that Unix systems share common principles, but there are subtle differences in how system calls behave and what headers are available. I had to study the differences between Linux and macOS, particularly with `clock_gettime()` and the `CLOCK_MONOTONIC` constant, to make sure my code worked correctly on both systems.

Understanding what happens behind each function was crucial to making them work correctly. When `fork()` is called, the operating system creates a new process with a copy of the parent's memory, and both parent and child continue executing from that point. The child process then calls `execvp()`, which replaces its memory with the new command, but the parent must use `wait()` or `waitpid()` to collect the child's exit status. With pipes, I learned that `pipe()` creates two file descriptors, and both parent and child must close their unused ends, otherwise the pipe won't signal EOF and the reading process will hang forever. For background jobs, I discovered that `waitpid()` with `WNOHANG` doesn't block—it returns immediately and lets the shell continue responding to new commands while checking background job status in the prompt. Each system call has specific behavior and requirements, and understanding these details was essential. I spent time studying Unix documentation and experimenting with small test programs to understand how `dup2()` actually redirects file descriptors, how `strtok()` modifies strings in place, and why you must close file descriptors in the correct order. This hands-on learning, combined with reading about Unix process management and inter-process communication, gave me a solid foundation in systems programming that goes beyond just making the code work—it helped me understand why it works the way it does.

## To compile:

```bash
gcc -Wall -Wextra -o enseash enseash.c background.c
```
## Execution

```bash    
./enseash
```
## Example Commads for Testing

```bash
# Welcome message
$ ./enseash
Welcome to ENSEA shell!!!!!!!
To quit, write: 'exit'
enseash %

# Simple command
enseash % echo hello
hello
enseash [exit:0|2ms] % 

# Command with arguments
enseash [exit:0|2ms] % hostname -i
10.10.2.245
enseash [exit:0|3ms] % 

# Redirection
enseash [exit:0|3ms] % ls > filelist.txt
enseash [exit:0|1ms] % wc -l < filelist.txt
44
enseash [exit:0|4ms] % 

# Pipe
enseash [exit:0|4ms] % ls | wc -l
44
enseash [exit:0|5ms] % 

# Background job
enseash [exit:0|5ms] % sleep 10 &
[1] 3656
enseash [1&] % 
[1]+ Ended: sleep 10 &
enseash [exit:0|10s] % 

# Exit
enseash [exit:0|10s] % exit
Beijos, tchau!(Translation: kisses, bye!)
$ 
```
## Authors
 Ana Paula Oliveira da Nóbrega Costa

 
 Jessica Leal de Melo
