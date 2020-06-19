// Mark Pipko and Joshua Mimer
// I pledge my honor that I have abided by the Stevens Honor System.

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <ctype.h>
#include <math.h>

int total_count = 0; 
pthread_mutex_t lock; 

typedef struct arg_struct {     
	int start;
	int end; 
} thread_args; 

void display_usage(){
    printf("Usage: ./mtsieve -s <starting value> -e <ending value> -t <num threads>\n" );
}
bool is_integer(char *input) {
    int begin = 0, len = strlen(input);

    if (len >= 1 && input[0] == '-') {
        if (len < 2) {
            return false;
        }
        begin = 1;
    }
    for (int i = begin; i < len; i++) {
        if (!isdigit(input[i])) {
            return false;
        }
    }
    return true;
}


void *sieve(void *ptr) {
    int local_number_of_primes = 0;
	thread_args *thread = (thread_args *)ptr;
    int limit = sqrt(thread->end);
    int *low_primes;
    if(( low_primes = (int *)malloc((limit + 1)*sizeof(int))) == NULL){
      fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    int *high_primes;
    if(( high_primes = (int *)malloc((thread->end - thread->start + 1)*sizeof(int))) == NULL){
      fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    for(int i = 0; i < thread->end - thread->start + 1; i++){
        high_primes[i] = 1;
    }
    for(int i = 0; i < limit + 1; i++){
        low_primes[i] = 1;
    }
    int k = 0;
    for (int i = 2; i < limit; i++) { 
        if (low_primes[i] == 1) {
            low_primes[k] = i;
            k++;
            for (int j = i; i * j < limit; j++){ 
                low_primes[i*j] = 0; 
            } 
        } 
    } 	

	for(int i=0; i<k; i++){
        int initial = (int)ceil((double)thread->start/low_primes[i]) * low_primes[i] - thread->start;
        if (thread->start <= low_primes[i]){
            initial += low_primes[i];
        }
        for(int p = initial; p<thread->end - thread->start + 1; p+=low_primes[i]){
            high_primes[p] = 0;
        }
	}
    for (int i = thread->start; i <= thread->end; i++){
        if (high_primes[i - thread->start]) {
            int prime_number = i;
            int three_count = 0;
            while(prime_number > 0){
                if(prime_number % 10 == 3){
                    three_count++;
                }
                prime_number = prime_number / 10;
            }
            if(three_count >= 2){
                local_number_of_primes++;
            }
        }
    }
	
	int retval;
    if ((retval = pthread_mutex_lock(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot lock mutex. %s.\n", strerror(retval));
        free(low_primes);
    	free(high_primes);
        exit(EXIT_FAILURE);
    }
    total_count+=local_number_of_primes;
	if ((retval = pthread_mutex_unlock(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n", strerror(retval));
        free(low_primes);
    	free(high_primes);
        exit(EXIT_FAILURE);
    }
    free(low_primes);
    free(high_primes);
    pthread_exit(NULL);
}


int main(int argc, char **argv){
	if(argc == 1){
		display_usage();
		return EXIT_FAILURE;
	}

	bool sflag = false;
    bool eflag = false;
    bool tflag = false;
    int opt;
    extern char *optarg;
    int starting = 0;
    int ending = 0;
    int num = 0;
    char *something;
    opterr = 0;
    while((opt = getopt(argc, argv,"s:e:t:")) != -1){
        switch (opt){
            case 's':
                sflag = true;
                if(!is_integer(optarg)){
                	fprintf(stderr, "Error: Invalid input '%s' received for parameter '-s'.\n", optarg);
    				return EXIT_FAILURE;
                }
                if(strtol(optarg, &something, 10) > INT_MAX){
                    fprintf(stderr, "Error: Integer overflow for paramater '-s'.\n");
                    return EXIT_FAILURE;
                }
                starting = atoi(optarg);
                break;
            case 'e':
                eflag = true;
                if(!is_integer(optarg)){
                	fprintf(stderr, "Error: Invalid input '%s' received for parameter '-e'.\n", optarg);
    				return EXIT_FAILURE;
                }
                if(strtol(optarg, &something, 10) > INT_MAX){
                    fprintf(stderr, "Error: Integer overflow for paramater '-e'.\n");
                    return EXIT_FAILURE;
                }
                ending = atoi(optarg);
                break;
            case 't':
                tflag = true;
                if(!is_integer(optarg)){
                	fprintf(stderr, "Error: Invalid input '%s' received for parameter '-t'.\n", optarg);
    				return EXIT_FAILURE;
                }
                if(strtol(optarg, &something, 10) > INT_MAX){
                    fprintf(stderr, "Error: Integer overflow for paramater '-t'.\n");
                    return EXIT_FAILURE;
                }
                num = atoi(optarg);
                break;
            case '?':
                if (optopt == 'e' || optopt == 's' || optopt == 't') {
 					fprintf(stderr, "Error: Option -%c requires an argument.\n", optopt);
 				} else if (isprint(optopt)) {
 					fprintf(stderr, "Error: Unknown option '-%c'.\n", optopt);
 				} else {
 					fprintf(stderr, "Error: Unknown option character '\\x%x'.\n", optopt);
 				}
 				return EXIT_FAILURE;
        }
    }

    if(optind < argc){
    	fprintf(stderr, "Error: Non-option argument '%s' supplied.\n", argv[optind]);
    	return EXIT_FAILURE;
    }
    if(!sflag){
    	fprintf(stderr, "Error: Required argument <starting value> is missing.\n");
    	return EXIT_FAILURE;
    }
    if(starting < 2){
    	fprintf(stderr, "Error: Starting value must be >= 2.\n");
    	return EXIT_FAILURE;
    }
    if(!eflag){
    	fprintf(stderr, "Error: Required argument <ending value> is missing.\n");
    	return EXIT_FAILURE;
    }
    if(ending < 2){
    	fprintf(stderr, "Error: Starting value must be >= 2.\n");
    	return EXIT_FAILURE;
    }
    if(starting > ending){
    	fprintf(stderr, "Error: Ending value must be >= starting value.\n");
    	return EXIT_FAILURE;
    }
    if(!tflag){
    	fprintf(stderr, "Error: Required argument <num threads> is missing.\n");
    	return EXIT_FAILURE;
    }
    if(num < 1){
    	fprintf(stderr, "Error: Numbers of threads cannot be less than 1.\n");
    	return EXIT_FAILURE;
    }
    if(num > 2 * get_nprocs()){
    	fprintf(stderr, "Error: Numbers of threads cannot exceed twice the number of processors(%d).\n", get_nprocs());
    	return EXIT_FAILURE;
    }

    int retval;
    if ((retval = pthread_mutex_init(&lock, NULL)) != 0) {
        fprintf(stderr, "Error: Cannot create mutex. %s.\n", strerror(retval));
        return EXIT_FAILURE;
    }


    pthread_t threads[num];
    thread_args targs[num];
    // if((ending - starting + 1) % 2 == 0){
    //  range = (int)(double)(ending - starting) / (double)(num);
    // }
    // else{
    //  range = (int)(double)(ending - starting + 1) / (double)(num);
    // }
    int range = (ending - starting + 1) / (num);  
    int start = starting;
    int end = starting + range;

    printf("Finding all prime numbers between %d and %d.\n", starting, ending);

    int temp = (ending - starting + 1) % (num);
    //printf("%d\n", temp );
    if(num > (ending - starting + 1)){
        num = (ending - starting + 1);
        range = 1;
    }
    for (int i = 0; i < num; i++) {
        if(temp == 0){
            end--;
        }
        if(temp > 0){
            temp--;
        }

        if(start > end){
            num = i+1;
            break;
        }

        targs[i].start = start;
        targs[i].end = end;

        start = end + 1;
        //range = ((ending - start + 1) / (num-i));
        end = start + range;

        if(end > ending){
            end = ending + 1;
        }


    }

    if(num > 1){
        printf("%d segments:\n", num);
    }
    else{
        printf("%d segment:\n", num);
    }
    for(int m = 0; m < num; m++){
        printf("   [%d, %d]\n", targs[m].start, targs[m].end);
        if ((retval = pthread_create(&threads[m], NULL, sieve,&targs[m])) != 0) {
            fprintf(stderr, "Error: Cannot create thread %d. %s.\n", m + 1, strerror(retval));
            return EXIT_FAILURE;
        }

    }
    for (int i = 0; i < num; i++) { /// IS THIS RIGHTTTTTTT
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Warning: Thread %d did not join properly.\n",
                    i + 1);
        }
    }

    if ((retval = pthread_mutex_destroy(&lock)) != 0) {
        fprintf(stderr, "Error: Cannot destroy mutex. %s.\n", strerror(retval));
    }

    printf("Total primes between %d and %d with two or more '3' digits: %d\n", starting, ending, total_count);

    return EXIT_SUCCESS;
}