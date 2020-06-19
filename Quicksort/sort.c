/*******************************************************************************
 * Name        : sort.c
 * Author      : Mark Pipko
 * Date        : Feburary 18, 2020
 * Description : Uses quicksort to sort a file of either ints, doubles, or
 *               strings.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort.h"
#include <stdbool.h>

#define MAX_STRLEN     64 // Not including '\0'
#define MAX_ELEMENTS 1024
#define BUFSIZE MAX_ELEMENTS

typedef enum {
    STRING,
    INT,
    DOUBLE
} elem_t;

/**
 * Basic structure of sort.c:
 *
 * Parses args with getopt.
 * Opens input file for reading.
 * Allocates space in a char** for at least MAX_ELEMENTS strings to be stored,
 * where MAX_ELEMENTS is 1024.
 * Reads in the file
 * - For each line, allocates space in each index of the char** to store the
 *   line.
 * Closes the file, after reading in all the lines.
 * Calls quicksort based on type (int, double, string) supplied on the command
 * line.
 * Frees all data.
 * Ensures there are no memory leaks with valgrind. 
 */

void display_usage(){
    printf("Usage: ./sort [-i|-d] [filename]\n"
            "   -i: Specifies the file contains ints.\n"
            "   -d: Specifies the file contains doubles.\n"  
            "   filename: The file to sort.\n"    
            "   No flags defaults to sorting strings.\n" );
}

int main(int argc, char **argv) {
    // char *arr = malloc(sizeof(char)*(5));
    // *(arr+0) = 'h';
    // *(arr+1) = 'a';
    // *(arr+2) = 'z';
    // *(arr+3) = 'q';
    // quicksort(arr,4,sizeof(char),str_cmp);
    // for(int i=0;i<4;i++){
    //  printf("%d\n",*(arr+i));
    // }

    if(argc == 1){
        display_usage();
        return 1;
    }
    bool integer = false;
    bool doublee = false;
    bool string = false;
    int opt;
    while((opt = getopt(argc, argv,":id")) != -1){
        switch (opt){
            case 'i':
                integer = true;
                break;
            case 'd':
                doublee = true;
                break;
            case '?':
                printf("Error: Unknown option '%s' received.\n", argv[1]);
                display_usage();
                return 1; 
        }
    }
    if(argc == 2){
        string = true;
    }
    FILE *fp;
    if(!string){
        fp = fopen(argv[2], "r");
    }
    else{
        fp = fopen(argv[1], "r");
    }
    if(fp == NULL){
        if(!string){
            fprintf(stderr, "Error: Cannot open file '%s'. No such file or directory.", argv[2]);
            return 1;
        }
        else{
            fprintf(stderr, "Error: Cannot open file '%s'. No such file or directory.", argv[1]);
            return 1;
        }
    }

    int index=0;
    char buf[MAX_ELEMENTS];

    if(integer){
        int *arr = malloc(sizeof(int)*(MAX_ELEMENTS));
        while (fgets(buf, MAX_STRLEN + 2, fp) && integer) {
            char *eoln = strchr(buf, '\n');
             if (eoln == NULL) {
                // This should not happen.
                buf[MAX_STRLEN] = '\0';
                } else {
                *eoln = '\0';
            }
            int n = atoi(buf);
            *(arr + index) = n;
            index++;
        }
        quicksort(arr,index,sizeof(int),int_cmp);
        for(int i=0;i<index;i++){
            printf("%d\n",*(arr+i));
        }
        free(arr);
    }
    else if(doublee){
        double *arr = malloc(sizeof(double)*(MAX_ELEMENTS));
        while (fgets(buf, MAX_STRLEN + 2, fp) && doublee) {
            char *eoln = strchr(buf, '\n');
             if (eoln == NULL) {
                // This should not happen.
                buf[MAX_STRLEN] = '\0';
                } else {
                *eoln = '\0';
            }
            double n = atof(buf);
            *(arr + index) = n;
            index++;
        }
        quicksort(arr,index,sizeof(double),dbl_cmp);
        //printf("%d",index);
        for(int i=0;i<index;i++){
            printf("%f\n",*(arr+i));
        }
        free(arr);
    }
    else{
        char **arr = (char **)malloc(sizeof(char*)*(MAX_ELEMENTS+2));
        for(int i=0; i<MAX_ELEMENTS+2; i++){
            arr[i] = (char *)malloc(sizeof(char)*(MAX_ELEMENTS+2));
        }
        while (fgets(buf, MAX_STRLEN + 2, fp) && string) {
            //buf = malloc(sizeof(int)*(MAX_STRLEN+2));
            char *eoln = strchr(buf, '\n');
             if (eoln == NULL) {
                // This should not happen.
                buf[BUFSIZE] = '\0';
                } else {
                *eoln = '\0';
            }
            // char n = *(buf);
            // *(arr + index) = n;
            strcpy(*(arr+index),buf);
            index++;
        }
        quicksort(arr,index,sizeof(char*),str_cmp);
        for(int i=0;i<index;i++){
            printf("%s\n",*(arr+i));
        }
        for(int i=0; i<MAX_ELEMENTS+2; i++){
            free(arr[i]);
        }
        free(arr);
    }

    fclose(fp);
    return 0;
}
