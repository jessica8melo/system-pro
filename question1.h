#ifndef ENSEASH_H
#define ENSEASH_H

#include <unistd.h> // unix: read, write, fork, execlp
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <time.h>


#define PROMPT "enseash % "
#define BUFFER_SIZE 1024
#define MAX_ARGS 32


#define WELCOME_MSG "Bem vindo a o shell da ENSEA!!!!!!!\n Pra quitar, escreva: 'exit'\n"
#define GOODBYE_MSG "Beijos, tchau!\n"


extern int last_status;
extern int last_signal;
extern long last_execution_time_ms;

int read_command(char* buffer);
void execute_command(const char* cmd);
void print_random_fortune();
void build_prompt(char* prompt_buf);
long calculate_execution_time_ms(struct timespec start, struct timespec end);


#endif