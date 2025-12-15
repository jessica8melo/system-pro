#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BG_JOBS 32
#define BUFFER_SIZE 1024

// background job struct
typedef struct {
    pid_t pid;
    int job_id;
    char command[BUFFER_SIZE];
    int status;  
    long execution_time_ms;
} background_job_t;

// jobs array
extern background_job_t background_jobs[MAX_BG_JOBS];
extern int num_bg_jobs;

int get_next_job_id(void);
void add_background_job(pid_t pid, const char* command);
void check_background_jobs(void);
int get_running_jobs_count(void);
void print_background_jobs_status(void);

#endif