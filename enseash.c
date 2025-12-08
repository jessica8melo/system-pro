#include "question1.h"

int last_status = 0;
int last_signal = 0;
long last_execution_time_ms = 0;

/**
 * Function: read_command - Reads a command from the user via terminal
 * Parameters: buffer - Buffer to store the command
 * Return: 0 on success, -1 on failure
 */
int read_command(char* buffer) {
    char prompt_buf[BUFFER_SIZE];
    build_prompt(prompt_buf);

    write(STDOUT_FILENO, prompt_buf, strlen(prompt_buf));
    ssize_t bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
    if (bytes <= 0) {
        return -1;
    } 
    buffer[bytes] = '\0';
    char* nl = strchr(buffer, '\n');

    if (nl) {
        *nl = '\0';
    }
    return 0;
}

/**
 * Function: build_prompt - Builds the shell prompt with status and execution time
 * Parameters: prompt_buf - Buffer to store the prompt string
 * Return: void
 */
void build_prompt(char* prompt_buf) {
    if (last_signal != 0) {
        snprintf(prompt_buf, BUFFER_SIZE, "enseash [sign:%d|%ldms] %% ", 
                 last_signal, last_execution_time_ms);
    } else {
        snprintf(prompt_buf, BUFFER_SIZE, "enseash [exit:%d|%ldms] %% ", 
                 last_status, last_execution_time_ms);
    }
}

/**
 * Function: execute_command - Executes the command entered by the user
 * Parameters: cmd - Command string to execute
 * Signal handling and execution time measurement 
 * Return: void
 */
void execute_command(const char* cmd) {
    if (strcmp(cmd, "exit") == 0) {
        write(STDOUT_FILENO, GOODBYE_MSG, strlen(GOODBYE_MSG));
        exit(0); 
    }

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    pid_t pid = fork();
    if (pid == 0) {
        execlp(cmd, cmd, (char*)NULL);
        exit(127);
    } else if (pid > 0) {
        int status;
        wait(&status);
        
        clock_gettime(CLOCK_MONOTONIC, &end_time);


        // calculate execution time in milliseconds
        long seconds_diff = end_time.tv_sec - start_time.tv_sec;
        long nanoseconds_diff = end_time.tv_nsec - start_time.tv_nsec;
        last_execution_time_ms = seconds_diff * 1000 + nanoseconds_diff / 1000000;
        

        // how the last process ended
        if (WIFEXITED(status)) {
            last_status = WEXITSTATUS(status);
            last_signal = 0;
        } else if (WIFSIGNALED(status)) {
            last_signal = WTERMSIG(status);
            last_status = 0;
        }
    }
}

/**
 * Function: main - Main function that implements the shell loop
 * welcome message and continuously reads and executes commands
 * no parameters
 * Return: 0 on exit
 */
int main() {
    char buffer[BUFFER_SIZE];
    write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG));

    while (1) {
        if (read_command(buffer) == -1) {
            return 0;
        } 
        if (strlen(buffer) > 0) {
            execute_command(buffer);
        } 
    }
    return 0;
}