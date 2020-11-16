/**
 * Alex Giacobbi
 * CPSC 346-02
 * Project 5
 * 11/12/2020
 * 
 * Demonstrates a multithreaded producer/consumer using semaphores
 * To compile: gcc buffer.c -pthread
 * To run: ./a.out <sleeptime> <num producer threads> <num consumer threads>
 */

#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1

buffer_item buffer[BUFFER_SIZE];
/*define semaphores and mutex*/
sem_t empty, full;
pthread_mutex_t lock;

int insertPointer = 0, removePointer = 0;

void *producer(void *param);
void *consumer(void *param);

int insert_item(buffer_item item)
{
    /* Acquire Empty Semaphore */
	sem_wait(&empty);
	
	/* Acquire mutex lock to protect buffer */
	pthread_mutex_lock(&lock);
	buffer[insertPointer] = item;
	if (++insertPointer >= BUFFER_SIZE)
		insertPointer = 0;

	/* Release mutex lock and full semaphore */
	pthread_mutex_unlock(&lock);
	sem_post(&full);

	return 0;
}

int remove_item(buffer_item *item)
{
	/* Acquire Full Semaphore */
	sem_wait(&full);	 

	/* Acquire mutex lock to protect buffer */
	pthread_mutex_lock(&lock);
	*item = buffer[removePointer];
	if (++removePointer >= BUFFER_SIZE)
		removePointer = 0;

	/* Release mutex lock and empty semaphore */
	pthread_mutex_unlock(&lock);
	sem_post(&empty);

	return 0;
}


int main(int argc, char *argv[])
{
	int sleepTime, producerThreads, consumerThreads;
	int i, j;

	if(argc != 4)
	{
		fprintf(stderr, "Useage: <sleep time> <producer threads> <consumer threads>\n");
		return -1;
	}

	/* Call atoi to get arguments */
	sleepTime = atoi(argv[1]);
	producerThreads = atoi(argv[2]);
	consumerThreads = atoi(argv[3]);


	/* Initialize semaphores and mutex */
    sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	pthread_mutex_init(&lock, NULL);
 

	/* Create the producer and consumer threads */
	pthread_t producers[producerThreads];
	for(i = 0; i < producerThreads; i++)
	{
		pthread_create(&producers[i], 0, producer, NULL);
	}
 
	pthread_t consumers[consumerThreads];
	for(j = 0; j < consumerThreads; j++)
	{
		pthread_create(&consumers[j], 0, consumer, NULL);
	}

	/* Sleep for user specified time */
	sleep(sleepTime);
	return 0;
}

void *producer(void *param)
{
	buffer_item item;

	while (TRUE)
	{
		sleep(rand() % 5);
		item = rand();
		if (insert_item(item))
			fprintf(stderr, "report error condition, PRODUCER");
		else
			printf("Producer produced %d\n", item);		
	}
}

void *consumer(void *param)
{
	buffer_item item;

	while (TRUE)
	{
		sleep(rand() % 5);
		if (remove_item(&item))
			fprintf(stderr, "report error condition, CONSUMER");
		else
			printf("Consumer consumed %d\n", item);		
	}
}