#include "enseash.h"
#include "background.h"

int last_status = 0;
int last_signal = 0;
long last_execution_time_ms = 0;


/**
 *Function: redirect_file_descriptor - redirects a file descriptor to a file
 * Parameters: old_fd - File descriptor to redirect (STDIN_FILENO or STDOUT_FILENO)
 *             filename - File to redirect to
 *             flags - Open flags (O_RDONLY, O_WRONLY | O_CREAT | O_TRUNC, etc.)
 *             mode - File permissions (0644)
 * Return: void
 */
void redirect_file_descriptor(int old_fd, const char* filename, int flags, mode_t mode) {
    int new_fd = open(filename, flags, mode);
    if (new_fd < 0) {
        perror(filename);
        exit(1);
    }
    dup2(new_fd, old_fd);
    close(new_fd);
}


/**
 * Function: parse_command - parses command string into argv and redirections
 * Parameters: cmd - Command string
 *             argv - Array to store arguments
 *             input_file - Pointer to input file (or NULL)
 *             output_file - Pointer to output file (or NULL)
 *             background - Pointer to background flag (1 if &, else 0)
 * Return: void
 */
void parse_command(char* cmd, char* argv[], char** input_file, char** output_file, int* background) {
    char* token = strtok(cmd, " \t\n");
    int argc = 0;
    *background = 0;

    while (token != NULL && argc < MAX_ARGS - 1) {
        if (strcmp(token, "&") == 0) {
            *background = 1;
        } else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t\n");
            if (token != NULL) {
                *input_file = token;
            }
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t\n");
            if (token != NULL) {
                *output_file = token;
            }
        } else {
            argv[argc++] = token;
        }
        token = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;
}

/**
 * Function: split_by_pipe - Splits a command string by pipe character '|'
 * Parameters: cmd - Command string
 *             commands - Array to store split commands
 *             num_commands - Pointer to store number of commands
 * Return: void
 */
void split_by_pipe(char* cmd, char* commands[], int* num_commands) {
    *num_commands = 0;
    char* pipe_token = strtok(cmd, "|");
    
    while (pipe_token != NULL && *num_commands < MAX_COMMANDS) {
        commands[*num_commands] = pipe_token;
        (*num_commands)++;
        pipe_token = strtok(NULL, "|");
    }
}

/**
 * Function: execute_pipeline - Executes a pipeline of commands
 * Parameters: commands - Array of command strings (separated by |)
 *             num_commands - Number of commands in pipeline
 *             input_file - Input file for first command (or NULL)
 *             output_file - Output file for last command (or NULL)
 * Return: void
 */
void execute_pipeline(char* commands[], int num_commands, char* input_file, char* output_file) {
    pid_t pids[MAX_COMMANDS];
    int pipes[MAX_COMMANDS - 1][2];
    
    // Create all pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            return;
        }
    }
    
    // fork and execute each command
    for (int i = 0; i < num_commands; i++) {
        char* argv[MAX_ARGS];
        char* dummy_input = NULL;
        char* dummy_output = NULL;
        int dummy_bg = 0;

        
        // strtok modifies the string
        char cmd_copy[BUFFER_SIZE];
        strcpy(cmd_copy, commands[i]);

        
        parse_command(cmd_copy, argv, &dummy_input, &dummy_output, &dummy_bg);
        
        if (argv[0] == NULL) {
            continue;
        }
        
        pids[i] = fork();
        if (pids[i] == 0) {
            // Child process: setup redirections
            
            // Redirect stdin from previous pipe or input file
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            } else if (input_file != NULL) {
                redirect_file_descriptor(STDIN_FILENO, input_file, O_RDONLY, 0);
            }
            
            // Redirect stdout to next pipe or output file
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            } else if (output_file != NULL) {
                redirect_file_descriptor(STDOUT_FILENO, output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            
            // Close all pipes in child process
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            // Execute command
            execvp(argv[0], argv);
            perror(argv[0]);
            exit(127);
        }
    }
    
    // Parent process: close all pipes
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Wait for all children and get final status
    int final_status = 0;
    for (int i = 0; i < num_commands; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (i == num_commands - 1) {
            final_status = status;
        }
    }
    
    update_status(final_status);
}




/**
 * Function: measure_execution_time - Measures execution time between two timespecs
 * Parameters: start_time - Start time
 *             end_time - End time
 * Return: void
 */
void measure_execution_time(struct timespec start_time, struct timespec end_time) {
    long seconds_diff = end_time.tv_sec - start_time.tv_sec;
    long nanoseconds_diff = end_time.tv_nsec - start_time.tv_nsec;
    last_execution_time_ms = seconds_diff * MILIS_PER_SEC + nanoseconds_diff / NANOS_PER_MILI;
}

/**
 * Function: update_status - Updates last_status and last_signal based on process status
 * Parameters: status - Status from wait()
 * Return: void
 */
void update_status(int status) {
    if (WIFEXITED(status)) {
        last_status = WEXITSTATUS(status);
        last_signal = 0;
    } else if (WIFSIGNALED(status)) {
        last_signal = WTERMSIG(status);
        last_status = 0;
    }
}

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

    if (nl) *nl = '\0';
    return 0;
}

/**
 * Function: build_prompt - Builds the shell prompt with status and execution time
 * Parameters: prompt_buf - Buffer to store the prompt string
 * Return: void
 */
void build_prompt(char* prompt_buf) {

    check_background_jobs();
    int running_jobs = get_running_jobs_count();


    if (running_jobs > 0) {
        if (last_signal != 0) {
            snprintf(prompt_buf, BUFFER_SIZE, "enseash [%d&|sign:%d|%ldms] %% ", running_jobs, last_signal, last_execution_time_ms);
        } else {
            snprintf(prompt_buf, BUFFER_SIZE, "enseash [%d&|exit:%d|%ldms] %% ", running_jobs, last_status, last_execution_time_ms);
        }
    } else {
        if (last_signal != 0) {
            snprintf(prompt_buf, BUFFER_SIZE, "enseash [sign:%d|%ldms] %% ", last_signal, last_execution_time_ms);
        } else {
            snprintf(prompt_buf, BUFFER_SIZE, "enseash [exit:%d|%ldms] %% ", last_status, last_execution_time_ms);
        }
    }
}

/**
 * Function: execute_command - Executes the command entered by the user, handles signals 
 *           and executes time measurement 
 * Parameters: cmd - Command string to execute
 * Return: void
 */
void execute_command(char* cmd) {
    if (strcmp(cmd, "exit") == 0) {
        write(STDOUT_FILENO, GOODBYE_MSG, strlen(GOODBYE_MSG));
        exit(0);
    }

    // special command: jobs
    if (strcmp(cmd, "jobs") == 0) {
        print_background_jobs_status();
        return;
    }

    // strtok modifies the string
    char cmd_for_split[BUFFER_SIZE];
    strcpy(cmd_for_split, cmd);
    
    // split command by pipes
    char* commands[MAX_COMMANDS];
    int num_commands = 0;
    split_by_pipe(cmd_for_split, commands, &num_commands);
    
    if (num_commands == 0) {
        return;
    }
    
    char commands_copy[MAX_COMMANDS][BUFFER_SIZE];
    for (int i = 0; i < num_commands; i++) {
        strcpy(commands_copy[i], commands[i]);
        commands[i] = commands_copy[i];  // point to persistent copy
    }
    
    // extract redirections from original command
    char* input_file = NULL;
    char* output_file = NULL;
    int background = 0;
    
    // make copies before parsing
    char original_copy[BUFFER_SIZE];
    strcpy(original_copy, cmd);
    
    char* dummy_argv[MAX_ARGS];
    parse_command(original_copy, dummy_argv, &input_file, &output_file, &background);
    
    // Start timing
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_UPTIME, &start_time);
    
    if (num_commands == 1) {
        // single command (no pipe)
        char* argv[MAX_ARGS];
        
        // make a copy before parsing
        char cmd_copy[BUFFER_SIZE];
        strcpy(cmd_copy, commands[0]);
        
        int temp_bg = 0;
        parse_command(cmd_copy, argv, &input_file, &output_file, &temp_bg);
        
        if (argv[0] == NULL) {
            return;
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            // child process
            if (input_file != NULL) {
                redirect_file_descriptor(STDIN_FILENO, input_file, O_RDONLY, 0);
            }
            if (output_file != NULL) {
                redirect_file_descriptor(STDOUT_FILENO, output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            
            execvp(argv[0], argv);
            perror(argv[0]);
            exit(127);
        } else if (pid > 0) {
            if (background) {
                // Background execution
                add_background_job(pid, cmd);
            } else {
                // parent process - foreground execution
                int status;
                wait(&status);
                clock_gettime(CLOCK_UPTIME, &end_time);
                measure_execution_time(start_time, end_time);
                update_status(status);
            }
        }
    } else {
        // Pipeline (multiple commands)
        execute_pipeline(commands, num_commands, input_file, output_file);
        clock_gettime(CLOCK_UPTIME, &end_time);
        measure_execution_time(start_time, end_time);
    }
}

/**
 * Function: main - Main function that implements the shell loop(welcome message and 
 *                  continuously reads and executes commands)
 * Parameters: none
 * Return: 0 on exit
 */
int main() {
    char buffer[BUFFER_SIZE];
    write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG));

    while (1) {
        if (read_command(buffer) == -1) return 0;
        if (strlen(buffer) > 0) execute_command(buffer);
    }
    return 0;
}