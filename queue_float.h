/* Code copied from https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/
*  Modified by David Pirogovsky <dap263@cornell.edu> 12/10/19
*/
// C program for array implementation of queue 
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 

#define Q_TYPE float

// A structure to represent a queue 
struct QueueFloat 
{ 
	int front, rear, size; 
	unsigned capacity; 
	Q_TYPE* array; 
}; 

// function to create a queue of given capacity. 
// It initializes size of queue as 0 
struct QueueFloat* createQueueFloat(unsigned capacity) 
{ 
	struct QueueFloat* queue = (struct QueueFloat*) malloc(sizeof(struct QueueFloat)); 
	queue->capacity = capacity; 
	queue->front = queue->size = 0; 
	queue->rear = capacity - 1; // This is important, see the enqueue 
	queue->array = (Q_TYPE*) malloc(queue->capacity * sizeof(Q_TYPE)); 
	return queue; 
} 

// Queue is full when size becomes equal to the capacity 
int isFullFloat(struct QueueFloat* queue) 
{ return (queue->size == queue->capacity); } 

// Queue is empty when size is 0 
int isEmptyFloat(struct QueueFloat* queue) 
{ return (queue->size == 0); } 

// Function to add an item to the queue. 
// It changes rear and size 
void enqueueFloat(struct QueueFloat* queue, Q_TYPE item) 
{ 
	if (isFullFloat(queue)) 
		return; 
	queue->rear = (queue->rear + 1)%queue->capacity; 
	queue->array[queue->rear] = item; 
	queue->size = queue->size + 1; 
//	printf("%d enqueued to queue\n", item); 
} 

// Function to remove an item from queue. 
// It changes front and size 
Q_TYPE dequeueFloat(struct QueueFloat* queue) 
{ 
	if (isEmptyFloat(queue)) 
		return (Q_TYPE) INT_MIN; 
	Q_TYPE item = queue->array[queue->front]; 
	queue->front = (queue->front + 1)%queue->capacity; 
	queue->size = queue->size - 1; 
	return item; 
} 

// Function to get front of queue 
Q_TYPE frontFloat(struct QueueFloat* queue) 
{ 
	if (isEmptyFloat(queue)) 
		return (Q_TYPE) INT_MIN; 
	return queue->array[queue->front]; 
} 

// Function to get rear of queue 
Q_TYPE rearFloat(struct QueueFloat* queue) 
{ 
	if (isEmptyFloat(queue)) 
		return (Q_TYPE) INT_MIN; 
	return queue->array[queue->rear]; 
} 

/*
// Driver program to test above functions./ 
int main() 
{ 
	struct QueueFloat* queue = createQueueFloat(1000); 

	enqueue(queue, 10); 
	enqueue(queue, 20); 
	enqueue(queue, 30); 
	enqueue(queue, 40); 

	printf("%d dequeued from queue\n\n", dequeue(queue)); 

	printf("Front item is %d\n", front(queue)); 
	printf("Rear item is %d\n", rear(queue)); 

	return 0; 
} 
*/
