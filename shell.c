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

    //basic entries with only one command avail per line
    if (n == -1 || equal(line, "exit")) {// ^D results in n == -1 or user enters the exit command
      should_run = false;
      printf("...exiting...\n");
      continue;
    }else if (equal(line, "!!")){ //user enters repeat command
      repeatcall();
      continue;
    }else if(equal(line, "ascii")){ //user enters ascii art command
      printart();
      continue;
    }else if(equal(line, "")){ //empty line
      continue;
    }
    /*
    //handle piping 
    char *pipetoken;
    char *commands[10]; //25 is max tokeans allowed, increase for more (if needed)
    int numCommands = 0;

    pipetoken = strtok(line, "|");
    while(pipetoken != NULL){
      commands[numCommands++] = pipetoken;
      pipetoken = strtok(NULL, "|");
    }

    //start executing commands
    for(int i = 0; i < numCommands; i++){
      char* section = commands[i];
      printf("subsection: %s\n", section);

      int pipefd[2];
      if(i < numCommands - 1){ //create pipes for sections but last
        if(pipe(pipefd) == -1){
          printf("Error: Could not pipe.\n");
        }

      }




      processLine(section);
    }
    */

    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;

    // Check for input redirection (<)
    char *inputFile = NULL;

    if (strstr(line, "<")) {
        char *tokens[25];
        int numTokens = 0;
        char *token = strtok(line, " ");
        while (token != NULL) {
            tokens[numTokens++] = token;
            token = strtok(NULL, " ");
        }

        for (int j = 0; j < numTokens; j++) {
            if (strcmp(tokens[j], "<") == 0) {
                if (j + 1 < numTokens) {
                    inputFile = tokens[j + 1];
                    tokens[j] = NULL;
                    break;
                }
            }
        }

        // Open the input file and set it as the input
        if (inputFile != NULL) {
            input_fd = open(inputFile, O_RDONLY);
            if (input_fd < 0) {
                //perror("open");
                //exit(EXIT_FAILURE);
            }
        }
    }

    // Check for output redirection (>)
    char *outputFile = NULL;
    if (strstr(line, ">")) {
        char *tokens[25];
        int numTokens = 0;
        char *token = strtok(line, " ");
        while (token != NULL) {
            tokens[numTokens++] = token;
            token = strtok(NULL, " ");
        }

        for (int j = 0; j < numTokens; j++) {
            if (strcmp(tokens[j], ">") == 0) {
                if (j + 1 < numTokens) {
                    outputFile = tokens[j + 1];
                    tokens[j] = NULL;
                    break;
                }
            }
        }

        // Open the output file and set it as the output
        if (outputFile != NULL) {
            output_fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (output_fd < 0) {
                //perror("open");
                //exit(EXIT_FAILURE);
            }
        }
    }

    processLine(line, input_fd, output_fd);

    //handle history feature
    if(lastcall != NULL){
      free(lastcall);
    }
    lastcall = line;
  }
  free(line);
  return 0;
}

void processLine(char *line, int input_fd, int output_fd){
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



  //handle file directing
  //input
  if(strstr(line, "<")){
    if(input_fd != STDIN_FILENO){
      dup2(input_fd, STDIN_FILENO);
      close(input_fd);
    }

  }
  //output
  if(strstr(line, "<")){
    if(output_fd != STDOUT_FILENO){
      dup2(output_fd, STDOUT_FILENO);
      close(output_fd);
    }

  }




  //fork process
  pid_t pid = fork();

  if(pid < 0){
    printf("Error: Could not fork.\n");
    exit(-1);
  }else if(pid > 0){ //parent branch
    wait(NULL);
  }else{  //child branch
    execute(args);
  }

}

//execution process
void execute(char **args) { 
  execvp(args[0], args);


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
  int input_fd = STDIN_FILENO;
  int output_fd = STDOUT_FILENO;
  processLine(lastcall, input_fd, output_fd); //cant handle file direct with the repeat function
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
