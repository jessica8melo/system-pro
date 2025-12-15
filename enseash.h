#ifndef ENSEASH_H
#define ENSEASH_H

#include <unistd.h> // unix: read, write, fork, execlp
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <time.h>

#include <sys/stat.h>
#include <fcntl.h>


#define PROMPT "enseash % "
#define BUFFER_SIZE 1024
#define MAX_ARGS 32
#define CLOCK_UPTIME CLOCK_MONOTONIC
#define MAX_COMMANDS 10



#define WELCOME_MSG "Welcome to ENSEA shell!!!!!!!\n To quit, write: 'exit'\n"
#define GOODBYE_MSG "Beijos, tchau!(Translation: kisses, bye!)\n"


extern int last_status;
extern int last_signal;
extern long last_execution_time_ms;

int read_command(char* buffer);
void execute_command(char* cmd);
void build_prompt(char* prompt_buf);
void redirect_fd(int old_fd, const char* filename, int flags, mode_t mode);
void execute_pipeline(char* commands[], int num_commands, char* input_file, char* output_file);
void measure_execution_time(struct timespec start_time, struct timespec end_time);
void update_status(int status);
void parse_command(char* cmd, char* argv[], char** input_file, char** output_file);
void split_by_pipe(char* cmd, char* commands[], int* num_commands);


#endif