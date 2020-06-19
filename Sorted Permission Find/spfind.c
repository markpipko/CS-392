/*Joshua Mimer and Mark Pipko
I pledge my honor that I have abided by the Stevens Honor System.*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>

bool starts_with(const char *str, const char *prefix) {
    /* TODO:
       Return true if the string starts with prefix, false otherwise.
       Note that prefix might be longer than the string itself.
     */
    if(strlen(str) < strlen(prefix)){
      return false;
    }
    for(int i = 0; i < strlen(prefix); i++){
      if(str[i] != prefix[i]){
        return false;
      }
    }
    return true;

}

int main(int argc, char *argv[]){

	int pfind_to_sort[2], sort_to_parent[2];
	if (pipe(pfind_to_sort) < 0){
		fprintf(stderr, "Error: Cannot create pipe from pfind to sort. %s.\n", strerror(errno));
		return EXIT_FAILURE;
	}
    if (pipe(sort_to_parent) < 0){
		fprintf(stderr,"Error: Cannot create pipe from sort to spfind. %s.\n", strerror(errno));
		return EXIT_FAILURE;
	}

    pid_t pid[2];
    if ((pid[0] = fork()) < 0) {
        fprintf(stderr, "Cannot fork for pfind. %s.\n", strerror(errno));
        return EXIT_FAILURE;
        // pfind
    }
    else if(pid[0] == 0){
        close(pfind_to_sort[0]);
        dup2(pfind_to_sort[1], STDOUT_FILENO);

        // Close all unrelated file descriptors.
        close(sort_to_parent[0]);
        close(sort_to_parent[1]);
        if( execv("pfind", argv) == -1){
            fprintf(stderr, "Error: pfind failed.\n");
            return EXIT_FAILURE;
        }

    }    

    if ((pid[1] = fork()) < 0) {
        fprintf(stderr, "Cannot fork for sort.\n");
        return EXIT_FAILURE;
    }
    else if (pid[1] == 0){
        // sort
        close(sort_to_parent[0]);
        dup2(sort_to_parent[1], STDOUT_FILENO);
        dup2(pfind_to_sort[0], STDIN_FILENO);

        // Close all unrelated file descriptors.
        close(pfind_to_sort[1]);

        if( execlp("sort", "sort", NULL) == -1){
        	fprintf(stderr, "Error: sort failed.\n");
            return EXIT_FAILURE;
        }
    }

    close(pfind_to_sort[0]);
    close(pfind_to_sort[1]);
    dup2(sort_to_parent[0], STDIN_FILENO);
    close(sort_to_parent[1]);

    char buffer[8192];
    int bytes_read;
    int count = 0;
    char prefix[] = {"Usage"};
    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer)))){
        if (bytes_read == -1){
            fprintf(stderr, "Error: read() failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if(starts_with(buffer, prefix)){
            write(STDOUT_FILENO, buffer, bytes_read);
            return EXIT_SUCCESS;
        }
        for(int i=0; i<bytes_read; i++){
            if(*(buffer+i) == '\n'){
                count++;
            }
        }
        write(STDOUT_FILENO,buffer,bytes_read);
    }
    buffer[count] = '\0';
    int status;
    if(waitpid(pid[0], &status, WUNTRACED | WCONTINUED) == -1){
        fprintf(stderr, "Error: waitpid() failed for pfind.\n");
        return EXIT_FAILURE;
    }
    int nextstatus;
    if(waitpid(pid[1], &nextstatus, WUNTRACED | WCONTINUED) == -1){
        fprintf(stderr, "Error: waitpid() failed for sort.\n");
        return EXIT_FAILURE;
    }
    // printf("exited, status:%d\n", WIFEXITED(status));
    // printf("exited, status:%d\n", WEXITSTATUS(status));
    // printf("%ld\n", sizeof(buffer));
    if(WEXITSTATUS(status) == 0){
    	printf("Total matches: %d\n", count);
    }
   

    return !(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS);
}
