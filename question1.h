#ifndef ENSEASH_H
#define ENSEASH_H

#include <unistd.h> // unix: read, write, fork, execlp
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROMPT "enseash % "
#define BUFFER_SIZE 1024

int read_command(char* buffer);
void execute_command(const char* cmd);


#define WELCOME_MSG "Bem vindo a o shell da ENSEA!!!!!!!\n Pra quitar, escreva: 'exit'.\n"

#endif