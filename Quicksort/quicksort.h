/*******************************************************************************
 * Name        : quicksort.h
 * Author      : Mark Pipko
 * Date        : Feburary 17, 2020
 * Description : Quicksort header.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
/**
 * TODO - put all non-static function prototypes from quicksort.c inside
 * wrapper #ifndef.
 */

#include <stdio.h>

#ifndef QUICKSORT_HEADER_H
#define QUICKSORT_HEADER_H
void quicksort(void *array, size_t len, size_t elem_sz,
               int (*comp) (const void*, const void*));
int int_cmp(const void *a, const void *b); 
int dbl_cmp(const void *a, const void *b);
int str_cmp(const void *a, const void *b);
#endif
