#include "question1.h"


//unecessary function
void print_random_fortune() {
    const char* fortunes[] = {
        "You will pass power electronics.\n",
        "You will nail power electronics.\n",
        "Study more for electronics.\n"
    };
    int count = 3;
    srand(time(NULL)); // Seed with timestamp
    int idx = rand() % count; // Random index to the fortune
    write(STDOUT_FILENO, fortunes[idx], strlen(fortunes[idx]));
}




int read_command(char* buffer) {
    write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG)); // mensagem bemvindoos
    write(STDOUT_FILENO, PROMPT, strlen(PROMPT)); 
    ssize_t bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1); // leave space for enter
    if (bytes <= 0){
        return -1; // EOF or error
    } 
    buffer[bytes] = '\0';
    char* nl = strchr(buffer, '\n');

    if (nl) {
         *nl = '\0';
    }
    return 0;
}

void execute_command(const char* cmd) {
    if (strcmp(cmd, "exit") == 0) {
        write(STDOUT_FILENO, GOODBYE_MSG, strlen(GOODBYE_MSG));
        exit(0); 
    }
    if (strcmp(cmd, "elctronics-fortune") == 0) {
        print_random_fortune();
        return;
    }


    pid_t pid = fork();
    if (pid == 0) {
        execlp(cmd, cmd, NULL); 
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    }
}



int main() {
    char buffer[BUFFER_SIZE];
    while (1) {
        if (read_command(buffer) == -1){
            return 0;
        } 
        if (strlen(buffer) > 0){
            execute_command(buffer);
        } 
    }
    return 0;
}