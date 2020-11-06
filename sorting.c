/**
 * Alex Giacobbi
 * Project 4 - Multi thread sort
 * 11/06/2020
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE				10
#define NUMBER_OF_THREADS	3

void *sorter(void *params);	/* thread that performs basic sorting algorithm */
void *merger(void *params);	/* thread that performs merging of results */

int list[SIZE] = {7,12,19,3,18,4,2,6,15,8};

int result[SIZE];

typedef struct {
	int from_index;
	int to_index;
} parameters;

int main (int argc, const char * argv[]) {
	int i;
    
	pthread_t workers[NUMBER_OF_THREADS];

    printf("Initial:  ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", list[i]);
    }
    printf("\n");

	/* establish the first sorting thread */
	//1. call malloc to allocate a “parameters”
	//2. use “parameters” to specify the first half of the array
    //3. create the first thread
    parameters *sortThreadParams1 = (parameters*) malloc(sizeof(parameters));
    sortThreadParams1 -> from_index = 0;
    sortThreadParams1 -> to_index = (SIZE / 2) - 1;
    pthread_create(&workers[0], 0, sorter, sortThreadParams1);
	

	/* establish the second sorting thread */
	//1. call malloc to allocate a “parameters”
	//2. use “parameters” to specify the first half of the array
    //3. create the second thread
    parameters *sortThreadParams2 = (parameters*) malloc(sizeof(parameters));
    sortThreadParams2 -> from_index = SIZE / 2;
    sortThreadParams2 -> to_index = SIZE - 1;
    pthread_create(&workers[1], 0, sorter, sortThreadParams2);
  
	
	/* now wait for the 2 sorting threads to finish */
	// use ptheread_join; wait for 2 sorting threads to finish 
	pthread_join(workers[0], NULL);
    pthread_join(workers[1], NULL);

	/* establish the merge thread */
	//reuse “parameters” to hold the beginning index in each half
	//create the third thread: merge 
    sortThreadParams1 -> from_index = 0;
    sortThreadParams1 -> to_index = SIZE / 2;
    pthread_create(&workers[2], 0, merger, sortThreadParams1);
	
	/* wait for the merge thread to finish */
	pthread_join(workers[2], NULL);


	/* output the sorted array */
	printf("Expected: 2 3 4 6 7 8 12 15 18 19\n");
    printf("Actual:   ");
	for (int i = 0; i < SIZE; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");

    return 0;
}

/**
 * Sorting thread.
 *
 * This thread can essentially use any algorithm for sorting
 */

void *sorter(void *params) {
	parameters* p = (parameters *)params;
	
	// Sorting algorithm here. It can be any sorting algorithm. (Insertion sort)
    int start = p -> from_index;
    int end = p -> to_index + 1;
    for (int i = start; i < end; i++) { 
        int cur = list[i]; 
        int j = i - 1; 
        while (j >= 0 && list[j] > cur) { 
            list[j + 1] = list[j]; 
            j--; 
        } 
        list[j + 1] = cur; 
    }

	pthread_exit(0);
}

/**
 * Merge thread
 *
 * Uses simple merge sort for merging two sublists
 */

void *merger(void *params) {
	parameters* p = (parameters *)params;
	
	//reuse “parameters” to hold the first index in each half
	//merge two sorted sublist to the array result
    int left = p -> from_index;
    int right = p -> to_index;
    int i = 0;

    while (left < p -> to_index && right < SIZE) {
        if (list[left] < list[right]) {
            result[i] = list[left];
            left++;
        } else {
            result[i] = list[right];
            right++;
        }
        i++;
    }


    while (left < p -> to_index) {
        result[i] = list[left];
        left++;
        i++;
    }


    while (right < SIZE) {
        result[i] = list[right];
        right++;
        i++;
    }
    
	
	pthread_exit(0);
}

