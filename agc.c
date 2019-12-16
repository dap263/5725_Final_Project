#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "queue.h"

#define HIST_SIZE 1024
#define MAX_SAMPLES 20000
#define READ_BUF_SIZE 1
#define READ_BUF_TYPE short

// USAGE: a is an array, i is index in the array to start at, d is -1 or +1
// sums five elements in the array, either 4 above or 4 below i
#define ARRAY_SUM(a, i, d)  a[i] + a[i + d] + a[i + 2*d] + a[i + 3*d] + a[i + 4*d]

int min = 1000; int max = 0;
// USAGE: a is an array, i is index 
void FIND_NEW_MIN(int a[HIST_SIZE], int i) { 
	while (!a[i]) {i+=1;};
	min = i; 
}
// USAGE: a is an array, i is index 
void FIND_NEW_MAX(int a[HIST_SIZE], int i) { 
	while (!a[i]) {i-=1;};
	max = i; 
}

int   a[HIST_SIZE];
int   width, middle;
char *fifo = "/home/pi/ECE-5725/Final-Project/adcBuf";

int main(int argc, char ** argv) {
	// INIT DATA STRUCTURES
	int fd, ret, val, bot, top, mid, tmp;
	struct Queue * q = createQueue(MAX_SAMPLES+10); // leave a little extra space	

	// READ
	READ_BUF_TYPE readBuf[READ_BUF_SIZE] = {};

	fd = open(fifo, O_RDONLY);
	if (fd == -1) {printf("couldn't open fifo\n"); return 1;}
	while (1) {	
	ret = read(fd, readBuf, READ_BUF_SIZE*sizeof(READ_BUF_TYPE));
	if (ret == -1)
		printf("read failed\n");
	else if (ret == 0) { printf("end of file\n"); return 0; }
	else {
		val = readBuf[0];
		
		// UPDATE
		a[val] += 1;
		if ( val > max ) { 
			max = val;
			printf("max now %d\n", max);
		}
		else if ( val < min) {
			min = val;
			printf("min now %d\n", min);
		}
		enqueue(q, val);
		if ( q->size > MAX_SAMPLES ) {
			tmp = dequeue(q);
			a[tmp] -= 1;
			if (!a[tmp]) { // this value is now zero
				if (tmp == min) { FIND_NEW_MIN(a, tmp); printf("old min: %d, new min: %d\n", tmp, min); }
				else if (tmp == max) { FIND_NEW_MAX(a, tmp); printf("old max: %d, new max: %d\n", tmp, max); }
			}
//		}

		// CHECK
		
	// assert (min - 5 > 0)
		// width = max - min;
		// if ( width > ( HIST_SIZE >> 1 ) ) { // only if the histogram is wide enough
		bot = ARRAY_SUM(a, min       ,  1);
		top = ARRAY_SUM(a, max       , -1);
		mid = ARRAY_SUM(a, middle - 2,  1);	

		if ( mid < (bot >> 1) || mid < (top >> 1) ) {}
		//	printf("d\n");
		else {}
		//	printf("n\n");
		}
	}
	}
	return 0;
}
