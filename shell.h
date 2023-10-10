#ifndef SHELL_H
#define SHELL_H

#include <assert.h>  // assert
#include <fcntl.h>   // O_RDWR, O_CREAT
#include <stdbool.h> // bool
#include <stdio.h>   // printf, getline
#include <stdlib.h>  // calloc
#include <string.h>  // strcmp
#include <unistd.h>  // execvp
#include <sys/wait.h> //wait
#include <sys/stat.h> //open
#include <fcntl.h> //open

#define MAXLINE 80
#define PROMPT "osh> "

#define RD 0
#define WR 1

//variables
extern char *lastcall;

//functions 
bool equal(char *a, char *b);
int fetchline(char **line);
int interactiveShell();
int runTests();
void execute(char **args, int argsSize);
int main();
void printart();
void repeatcall();
void invalidcall();
void processLine(char *line);
void pipedCall(char **parsed, char **parseargs);
bool checkForAmp(char **args, int argSize);
int checkForIO(char **args, int argSize, char **inFile, char **outFile);


#endif