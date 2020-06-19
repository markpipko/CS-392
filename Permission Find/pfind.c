/*******************************************************************************
 * Name        : pfind.c
 * Author      : Mark Pipko
 * Date        : Feburary 27, 2020
 * Description : This program will recursively search for files whose permissions match the permissions string starting in the specified directory.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
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


void display_usage(){
    printf("Usage: ./pfind -d <directory> -p <permissions string> [-h] \n" );
}

int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR,
               S_IRGRP, S_IWGRP, S_IXGRP,
               S_IROTH, S_IWOTH, S_IXOTH};
char *useme;  

char* permission_string(struct stat *statbuf) {
    int ind = 0;
    int permission_valid;
    useme = (malloc(sizeof(char)*10)); 
    for (int i = 0; i < 9; i += 3) {    
        permission_valid = statbuf->st_mode & perms[i];
        if (permission_valid) {
            useme[ind] = 'r';
            ind++;    
        } 
        else 
        {
            useme[ind] = '-';
            ind++;    
        }    
        permission_valid = statbuf->st_mode & perms[i+1];
        if (permission_valid) {
            useme[ind] = 'w'; 
            ind++;   
        } 
        else {
            useme[ind] = '-'; 
            ind++;   
        }    
        permission_valid = statbuf->st_mode & perms[i+2];
        if (permission_valid) {
            useme[ind] = 'x';
            ind++;    
        } 
        else {
            useme[ind] = '-';
            ind++;   
        }
    }
    return useme;
}

bool checkPrintString(char *pstring){
	size_t length = strlen(pstring);
	if(length != 9){
		return false;
	}
	for(size_t i=0; i<length; i++){
		if(i==0 || i==3 || i==6){
			if(pstring[i] != 'r' && pstring[i] != '-'){
				return false;
			}
		}
		if(i==1 || i==4 || i==7){
			if(pstring[i] != 'w' && pstring[i] != '-'){
				return false;
			}
		}
		if(i==2 || i==5 || i==8){
			if(pstring[i] != 'x' && pstring[i] != '-'){
				return false;
			}
		}
	}
	return true;

}

void printing(char *der,char *pstring){
    struct dirent *de;
    struct stat statbuf;

    char full_filename[PATH_MAX];
	size_t pathlen = 0;
	char path[PATH_MAX];

	realpath(der,path);

	if( strcmp(path, "/")){
		// If path is not the root - '/', then ...

		// If there is no NULL byte among the first n bytes of path,
		// thefull_filename will not be terminated.
		strncpy(full_filename, path, PATH_MAX-1);
	}

	pathlen = strlen(full_filename) + 1;
	full_filename[pathlen - 1] = '/';
	full_filename[pathlen] = '\0';

    DIR *dir = opendir(der);
	while ((de = readdir(dir)) != NULL) {
		if (strcmp (de->d_name, "..") == 0 || strcmp (de->d_name, ".") == 0) {
			continue;
		}
		strncpy(full_filename + pathlen, de->d_name, PATH_MAX-pathlen);

		if(de->d_type == DT_DIR){
			if(access(full_filename,R_OK) == -1){
				fprintf(stderr, "Error: Cannot open directory '%s'. Permission denied.\n", full_filename);
			}
		else{
			lstat(full_filename, &statbuf);
			if(strcmp(pstring,permission_string(&statbuf)) == 0){
				printf("%s\n",full_filename);
			}
			free(useme);
			printing(full_filename,pstring);
			}
		}
		else{
			lstat(full_filename, &statbuf);
			if(strcmp(pstring,permission_string(&statbuf)) == 0){
				printf("%s\n",full_filename);
			}
			free(useme);
		}
	}
	closedir(dir);
}

int main(int argc, char **argv){
	if(argc < 2){
		display_usage();
		exit(EXIT_FAILURE);
	}

	bool dflag = false;
    bool pflag = false;
    int opt;
    extern char *optarg;
    char* dname;
    char* pname;
    while((opt = getopt(argc, argv,":d:p:h")) != -1){
        switch (opt){
            case 'd':
                dflag = true;
                dname = optarg;
                break;
            case 'p':
                pflag = true;
                pname = optarg;
                break;
            case 'h':
                display_usage();
                exit(EXIT_SUCCESS);
            case '?':
                printf("Error: Unknown option '-%c' received.\n",optopt);
                exit(EXIT_FAILURE);
        }
    }

    if(!dflag){
    	printf("Error: Required argument -d <directory> not found.\n");
    	exit(EXIT_FAILURE);
    }
    if(!pflag){
    	printf("Error: Required argument -p <permissions string> not found.\n");
    	exit(EXIT_FAILURE);
    }
    
	if(access(dname,F_OK) == -1){
		fprintf(stderr, "Error: Cannot stat '%s'. No such file or directory.\n", dname);
		exit(EXIT_FAILURE);
	}
		char buf[PATH_MAX];
		realpath(dname,buf) ;
		if(access(dname,R_OK) == -1){
			fprintf(stderr, "Error: Cannot open directory '%s'. Permission denied.\n", buf);
		exit(EXIT_FAILURE);
	} 
	
	DIR *dir = opendir(dname);
	if(dir == NULL){
		fprintf(stderr, "Error: '%s' is not a directory.\n", dname);
		closedir(dir);
		exit(EXIT_FAILURE);
	}
	closedir(dir);



	if(!checkPrintString(pname)){
		printf("Error: Permissions string '%s' is invalid.\n",pname);
		exit(EXIT_FAILURE);
	}

	// if(checkPrintString(argv[4])){
	// 	printf("corect");
	// }

	printing(dname,pname);

	exit(EXIT_SUCCESS);
}
