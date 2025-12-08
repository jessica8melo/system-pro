#include "question1.h"

int read_command(char* buffer) {
    write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG)); // mensagem bemvindoos
    write(STDOUT_FILENO, PROMPT, strlen(PROMPT)); 
    ssize_t bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1); // 
    if (bytes <= 0){
        return -1;
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
        exit(0); 
    }

    pid_t pid = fork();
    if (pid == 0) {
        execlp(cmd, cmd, (char*)NULL);
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