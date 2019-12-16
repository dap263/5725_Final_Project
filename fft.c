/*

Written by: David Pirogovsky <dap263@cornell.edu> 12/4/19
gcc -Wall -fPIC -o afft fft.c kiss_fft/kiss_fft.c -lm

*/
#include <stdio.h>
#include <string.h>

// for opening fifo
#include <unistd.h>
#include <fcntl.h>

#define READ_BUF_SIZE 2048
#define LOOPS 20

// to time fifo
#include <time.h>
#define BILLION 1000000000

#include "kiss_fft/kiss_fft.h"

#define FIXED_POINT 32 // should set values in kiss_fft_cpx to int_32t

#define NFFT 2*1024
#define R_MAX 1024

int main(int argc, char **argv) {
	kiss_fft_cfg cfg = kiss_fft_alloc( NFFT, 0, 0, 0); // nfft, is_inverse_fft, 0, 0
	kiss_fft_cpx *buf;
	kiss_fft_cpx *bufout;
	buf = (kiss_fft_cpx*) KISS_FFT_MALLOC(NFFT*sizeof(kiss_fft_cpx));
	bufout = (kiss_fft_cpx*) KISS_FFT_MALLOC(NFFT*sizeof(kiss_fft_cpx));
	
	struct timespec start, end, totS, totE;
	double read_time, fft_time;
	char *fifo = "/home/pi/ECE-5725/Final-Project/adcBuf";
	int fd;
	fd = open( fifo, O_RDONLY );
	if ( fd == -1 ) { printf("couldn't open fifo\n"); return 1; }

	int i;
	for (i = 0; i < 6000000; i++) {printf(" ");};
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC, &totS);
	for (int idx = 0; idx < LOOPS; idx++) {
/*
	int size = 256;
	for (i = 0; i < NFFT; i++) { 
		buf[i].r = sin(6.283*i/size)+1;
		buf[i].i = 0;
	}
//buf[i].r = rand() % R_MAX;
*/
	int32_t max = 0;	
	short readBuf[READ_BUF_SIZE] = {};
/*
	for (i = 0; i < NFFT; i++) {
		if (i == 1) 
			clock_gettime(CLOCK_MONOTONIC, &start);	
		printf("read %i \t", readBuf[i]);
		buf[i].r = readBuf[i];
	}
*/

	clock_gettime(CLOCK_MONOTONIC, &start);	
	read( fd, readBuf, READ_BUF_SIZE*sizeof(short));
	for (i = 0; i < NFFT; i++) 
		buf[i].r = readBuf[i];
	clock_gettime(CLOCK_MONOTONIC, &end);
	read_time = BILLION *(end.tv_sec - start.tv_sec) +(end.tv_nsec - start.tv_nsec);
    read_time = read_time / BILLION;
	//while
	clock_gettime(CLOCK_MONOTONIC, &start);
	kiss_fft( cfg, buf, bufout );
	int *out = malloc(NFFT*sizeof(int));
	for (i = 0; i < (NFFT/2+1); i++) {
//		printf("cx_out %i: %f\n", i, bufout[i].r);	
		out[i] = abs(bufout[i].r);
		if (out[i] > max)
			max = out[i];
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	fft_time = BILLION *(end.tv_sec - start.tv_sec) +(end.tv_nsec - start.tv_nsec);
    fft_time = fft_time / BILLION;
	
	printf("FIFO: %d values, %7.6f seconds, FFT: %7.6f seconds, max: %d\n", NFFT, read_time, fft_time, max);
	}
	clock_gettime(CLOCK_MONOTONIC, &totE);

	read_time = BILLION *(totE.tv_sec - totS.tv_sec) +(totE.tv_nsec - totS.tv_nsec);
    read_time = read_time / BILLION;
	printf("Total time after setup and blank loop: %7.6f seconds\n", read_time);

	kiss_fft_free(cfg);
	return 0;
}
