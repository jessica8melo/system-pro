#include "background.h"

background_job_t background_jobs[MAX_BG_JOBS];
int num_bg_jobs = 0;


/**
 * Function: get_next_job_id - gets the next available job ID
 * Return: Next job ID
 */
int get_next_job_id(void) {
    int max_id = 0;
    for (int i = 0; i < num_bg_jobs; i++) {
        if (background_jobs[i].job_id > max_id) {
            max_id = background_jobs[i].job_id;
        }
    }
    return max_id + 1;
}

/**
 * Function: add_background_job - Adds a background job to the list
 * Parameters: pid - Process ID
 *             command - Command string
 * Return: void
 */
void add_background_job(pid_t pid, const char* command) {
    if (num_bg_jobs >= MAX_BG_JOBS) {
        fprintf(stderr, "Too many background jobs\n");
        return;
    }
    
    background_jobs[num_bg_jobs].pid = pid;
    background_jobs[num_bg_jobs].job_id = get_next_job_id();
    strcpy(background_jobs[num_bg_jobs].command, command);
    background_jobs[num_bg_jobs].status = 0;  // Running
    background_jobs[num_bg_jobs].execution_time_ms = 0;
    
    printf("[%d] %d\n", background_jobs[num_bg_jobs].job_id, pid);
    num_bg_jobs++;
}

/**
 * Function: check_background_jobs - checks status of background jobs
 * Uses non-blocking wait check if jobs are done
 * Return: void
 */
void check_background_jobs(void) {
    for (int i = 0; i < num_bg_jobs; i++) {
        if (background_jobs[i].status == 0) {  // still running
            int status;
            pid_t result = waitpid(background_jobs[i].pid, &status, WNOHANG);
            
            if (result == background_jobs[i].pid) {
                //job finished
                background_jobs[i].status = 1;
                
                if (WIFEXITED(status)) {
                    int exit_code = WEXITSTATUS(status);
                    printf("[%d]+ Ended: %s\n", background_jobs[i].job_id, background_jobs[i].command);
                } else if (WIFSIGNALED(status)) {
                    int sig = WTERMSIG(status);
                    printf("[%d]+ Terminated by signal %d: %s\n", 
                           background_jobs[i].job_id, sig, background_jobs[i].command);
                }
            }
        }
    }
}

/**
 * Function: get_running_jobs_count - Returns count of running background jobs
 * Return: Number of running jobs
 */
int get_running_jobs_count(void) {
    int count = 0;
    for (int i = 0; i < num_bg_jobs; i++) {
        if (background_jobs[i].status == 0) {
            count++;
        }
    }
    return count;
}

/**
 * Function: print_background_jobs_status - Prints status of all background jobs
 * Return: void
 */
void print_background_jobs_status(void) {
    printf("\nBackground jobs:\n");
    for (int i = 0; i < num_bg_jobs; i++) {
        char status_str[20];
        if (background_jobs[i].status == 0) {
            strcpy(status_str, "Running");
        } else {
            strcpy(status_str, "Done");
        }
        printf("[%d] %s - %s (PID: %d)\n", 
               background_jobs[i].job_id, status_str, background_jobs[i].command, background_jobs[i].pid);
    }
    printf("\n");
}