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


    processLine(line);
    if(lastcall != NULL){
      free(lastcall);
    }
    lastcall = line;

  }

  free(line);
  return 0;
}

void processLine(char *line){
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
