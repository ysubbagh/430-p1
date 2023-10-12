#include "shell.h"

//global variables
char *lastcall = NULL;

int main() {
  return interactiveShell();
  /*
  if (argc == 2 && equal(argv[1], "--interactive")) {
    return interactiveShell();
  } else {
    return runTests();
  }
  */
}

// interactive shell to process commands
int interactiveShell() {
  bool should_run = true; //infiinter loop to run the shell
  char *line = calloc(1, MAXLINE);
  while (should_run) {
    printf(PROMPT);
    fflush(stdout);
    int n = fetchline(&line);
    // ^D results in n == -1
    if(n == -1){
      should_run = false;
      continue;
    }

    //actual stuff
    should_run = processLine(line);
    
    //handle history feature
    if(lastcall != NULL){
      free(lastcall);
    }
    lastcall = line;

  }
  //free memory
  free(line);
  return 0;
}

bool processLine(char *line){
//basic entries with only one command avail per line
  if (equal(line, "exit")) {//user enters the exit command
    printf("...exiting...\n");
    return false;
  }else if (equal(line, "!!")){ //user enters repeat command
    repeatcall();
    return true;
  }else if(equal(line, "ascii")){ //user enters ascii art command
    printart();
    return true;
  }else if(equal(line, "")){ //empty line
    return true;
  }

  //tokenize
  char *token;
  char *args[25]; //25 is max tokeans allowed, increase for more (if needed)
  int argsize = 0;

  //create token arguments of user commannd into array 
  token = strtok(line, " ");
  while(token != NULL){
    args[argsize++] = token;
    token = strtok(NULL, " ");
  }
  args[argsize] = NULL;

  //check for & and ;
  bool runParent = checkForAmp(args, argsize);

  //fork process
  pid_t pid = fork();

  if(pid < 0){ //in wrong fork
    printf("Error: Could not fork.\n");
    return false;
  }else if(pid > 0){ //parent branch
    if(!runParent){ //only wait if there was no apmpersands
      wait(NULL);
    }
  }else{  //child branch

    //setup file stuff
    char *inFile, *outFile;
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;
    int redirect = checkForIO(args, argsize, inFile, outFile);
    if(redirect > -1){ //redirect found
      doRedirection(redirect, inFile, outFile, input_fd, output_fd);
    }

    //execute
    execute(args, argsize);

    fflush(stdin);
  }
  return true;
}

//execution process
void execute(char **args, int argsSize) { 
  execvp(args[0], args);
}

bool checkForAmp(char **args, int argSize){
  for(int i = 0; i <= argSize; i++){
    if(args[i] == NULL){
      break;
    }else if(equal(args[i], "&")){
      args[i] = NULL;
      argSize--;
      return true; 
    }
  }
  return false; //base case (either semicolon found, or no semicolon and no ampersands)
}

int checkForPipe(char **args, int argSize){
  for(int i = 0; i <= argSize; i++){
    if(args[i] == NULL){
      break;
    }else if(equal(args[i], "|")){
      return i; //return the position of the pipe
    }
  }
  return -1; //base case
}

int checkForIO(char **args, int argSize, char **inFile, char **outFile){
  for(int i = 0; i <= argSize; i++){
    if(args[i] == NULL){
      break;
    }else if(equal(args[i], "<")){ //input redirect found
      *inFile = args[i + 1];
      args[i] = NULL;
      args[i+1] = NULL; 
      argSize -= 2;
      return 0;
    }else if(equal(args[i], ">")){ //output redirect found
      *outFile = args[i+1];
      args[i] = NULL;
      args[i+1] = NULL; 
      argSize -= 2;
      return 1;
    }
  }
  return -1; //base case (no I/O redirect found)
}

void doRedirection(int type, char **inFile, char **outFile, int *input, int *output){
  if(type == 0){ //input redirection
    input = open(*inFile, O_RDONLY | 0666);
    dup2(input, STDIN_FILENO);
    close(input);
  }else if(type == 1){ //output redirection
    int newO = open(*outFile, O_WRONLY | O_CREAT | O_TRUNC| 0666);
    dup2(newO, STDOUT_FILENO);
    close(newO);
  }else{ //base case, something wrong
    printf("Error: Redirection issue.\n");
    exit(-1);
  }
}

void pipedCall(char **parsed, char **parseargs){
  int pipefd[2];
  pid_t p1, p2;

  if(pipe(pipefd) < 0){
    printf("Error: Could not pipe.\n");
    return;
  }

}

//print the ascii ski art
void printart(){
  printf("                       __\n");
  printf("                      (=[)\n");
  printf("                    /`\\ -.\n");
  printf("`` ,,``           /`| ,_,_`-._\n");
  printf("     ,,'         /  `---,)`--.)\n");
  printf("  ``,,  ''      (  '._-/_   /\n");
  printf(" ,,    `` ``,,   \\   /') )/'\n");
  printf("  , ``,''`;; ,,   `>' / / \n");
  printf(" `` '',`;;,`;;, -/' /|  \\\n");
  printf("    ``,; --..._/|  \\ ```---...___\n");
  printf("             /'  ```---...___   _```--'\n");
  printf("                             ```\n");
}

//history, repeat the last call
void repeatcall(){
  if(lastcall == NULL){ //no history yet
    printf("Error: No commands in history.\n");
    return;
  }
  processLine(lastcall); 
}

int runTests() {
  printf("*** Running basic tests ***\n");
  char lines[7][MAXLINE] = {
      "ls",      "ls -al", "ls & whoami ;", "ls > junk.txt", "cat < junk.txt",
      "ls | wc", "ascii"};
  for (int i = 0; i < 7; i++) {
    printf("* %d. Testing %s *\n", i + 1, lines[i]);
    //processLine(lines[i]);
  }

  return 0;
}

// return true if C-strings are equal
bool equal(char *a, char *b) { return (strcmp(a, b) == 0); }

// read a line from console
// return length of line read or -1 if failed to read
// removes the \n on the line read
int fetchline(char **line) {
  size_t len = 0;
  size_t n = getline(line, &len, stdin);
  if (n > 0) {
    (*line)[n - 1] = '\0';
  }
  return n;
}
