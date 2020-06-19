#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <pwd.h>
#include <sys/types.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <wait.h>

#define BRIGHTBLUE "\x1b[34;1m" 
#define DEFAULT    "\x1b[0m"
#define BUFSIZE 8192

sigjmp_buf jmpbuf;

volatile sig_atomic_t signal_val = 0;

void catch_signal(int sig) {
    if(!signal_val){
      write(STDOUT_FILENO, "\n", 1);
      siglongjmp(jmpbuf, 1);
    }
}

int main(int argc, char *argv[]) {
  if(argc != 1){
    fprintf(stderr, "Usage: %s\n", argv[0]);
    return EXIT_FAILURE;
  }
  struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = catch_signal;
    action.sa_flags = SA_RESTART; /* Restart syscalls if possible */

    if (sigaction(SIGINT, &action, NULL) < 0) {
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    
    do {
    sigsetjmp(jmpbuf, 1);
    char buf[BUFSIZE];
    char input[BUFSIZE];
  if(getcwd(buf, BUFSIZE) == NULL){
    fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
    return EXIT_FAILURE;
  }
  printf("[");
  printf("%s%s", BRIGHTBLUE, buf); 
  printf("%s]$ ", DEFAULT);
  fflush(stdout);
  // memset(input, 0, BUFSIZE);
  ssize_t bytes_read = read(STDIN_FILENO, input, sizeof(input)-1);
  if(bytes_read < 0){
  	fprintf(stderr, "Error: read() failed. %s.\n", strerror(errno));
  	return EXIT_FAILURE;
  }
    input[bytes_read-1] = '\0';
    // printf("Printing input after read: %s", input);
    if (bytes_read == 1) {
        continue;
    }
    char **arguments;
    if(( arguments = (char **)malloc(2048*sizeof(char*))) == NULL){
      fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
      return EXIT_FAILURE;
    }
    // for (int i = 0; i < 2048; i++){
    //     if(( arguments[i] = (char*)malloc(2048*sizeof(char))) == NULL){
    //       fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
    //       return EXIT_FAILURE;
    //     }
    // }
    char *temp = strtok(input, " ");
    int word_counter = 0;
    int i = 0;
    while (temp != NULL){
        if (( arguments[i] = (char*)malloc((strlen(temp)+1)*sizeof(char))) == NULL){
          fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
          return EXIT_FAILURE;
        }
        strcpy(arguments[i], temp);
        temp = (char*)strtok(NULL, " ");
        word_counter++;
        i++;
    }
    if (strcmp(arguments[0], "exit") == 0) {
      for (int j = 0; j < word_counter; j++){
        free(arguments[j]);
    }
    free(arguments);
        break;
    }
    else if(strcmp(arguments[0], "cd") == 0){
      if(word_counter > 2){
        fprintf(stderr, "Error: Too many arguments to cd.\n");
      }
      else{
       if( (word_counter == 1) || (strcmp(arguments[1], "~") == 0)){
          //we will be printing out the home directory
          uid_t uid = getuid();
          struct passwd *pw = getpwuid(uid);
          if(pw == NULL){
            fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
          }
          else{
            char homedir[PATH_MAX];
            strcpy(homedir, pw->pw_dir);
            if(chdir(homedir) < 0){
              fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", homedir, strerror(errno));
            }
          }
        }
        else{
          uid_t uid = getuid();
          struct passwd *pw = getpwuid(uid);
          if(pw == NULL){
            fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
          }
          char homedir[PATH_MAX];
          strcpy(homedir, pw->pw_dir);
          if(strchr(arguments[1], '~') != NULL){
            char *split = strtok(arguments[1], "~");
            char *final = strcat(homedir, split);
            if(chdir(final) < 0){
              fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", arguments[1], strerror(errno));
             }
            }
          else{
            if(chdir(arguments[1]) < 0){
              fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", arguments[1], strerror(errno));
            }
            }
        }
        
    }
  }
  
    else{
      pid_t pid;
      if ((pid = fork()) < 0) {
        fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
      }
      else if (pid > 0) {
      //parent
        signal_val = 1;
        int status; 
        pid_t p = waitpid(pid, &status, WUNTRACED | WCONTINUED);
        if(p == -1){
          fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
          return EXIT_FAILURE;
        }
        signal_val = 0;
    }
      else{
        char *a = arguments[0];
        a[strlen(a)] = '\0';
        // free(arguments[word_counter]);
        arguments[word_counter] = NULL;
        if(execvp(a, arguments) < 0){
          fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
          for (int j = 0; j < word_counter; j++){
            free(arguments[j]);
          }
          free(arguments);
          exit(EXIT_FAILURE);
        }
      }
    }
    for (int j = 0; j < word_counter; j++){
        free(arguments[j]);
    }
    free(arguments);


  }while (true);
    return EXIT_SUCCESS;
}