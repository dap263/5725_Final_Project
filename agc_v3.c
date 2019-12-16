/*
gcc agc_v3.c -lm -lwiringPi
*/
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <math.h>
#include <wiringPi.h>
//#include <wiringPiSPI.h>

#include "queue.h"
#include "queue_float.h"

#define RD_RANGE 1024
#define R_SIZE 20
#define INTERVAL 500
#define HOLD_TIME 2000
#define HIST_SIZE 256
#define MAX_SAMPLES 2*2048
#define READ_BUF_SIZE 1
#define READ_BUF_TYPE short
#define WRITE_BUF_SIZE 1
#define WRITE_BUF_TYPE unsigned short

#define MIN_GAIN 1950
#define MAX_GAIN 4095

// USAGE: a is an array, i is index in the array to start at, d is -1 or +1
// sums five elements in the array, either 4 above or 4 below i
int ARRAY_SUM(int a[HIST_SIZE], int i, int d) {
	return a[i] + a[i + d] + a[i + 2*d] + a[i + 3*d] + a[i + 4*d];
}

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
char *readFifo = "/home/pi/ECE-5725/Final-Project/adcBuf";
char *writeFifo = "/home/pi/ECE-5725/Final-Project/dacBuf";

short gainTarget = 4095;
short oldGT = 4095;
float thresh = 1;
float maxR = 0.0;
int cnt = 0;

float AR_MAX(float af[R_SIZE]) {
	float m = -1.0;
	for (int i = 0; i < R_SIZE; i++) {
		if (af[i] > m)
			m = af[i];
	}
	//printf("%5.4f\n", m);
	return m;
}
float af[R_SIZE];
int fPtr = 0;
float lMax, br, tr;
void update() {
	int	bot = ARRAY_SUM(a, min       ,  1);
	int top = ARRAY_SUM(a, max       , -1);
	int mid = ARRAY_SUM(a, middle - 2,  1);	
//	for (int i = 0; i < HIST_SIZE; i++)
//		printf("%d ", a[i]);
//	printf("\n");
	
//	printf("%d %d %d\n", bot,top,mid);
	if ( mid ) {
		br = (float) bot / (float) mid; 
		tr = (float) top / (float) mid;
	}
	if ( tr > br ) 
		lMax = tr;
	else
		lMax = br;
	af[fPtr++ % R_SIZE] = lMax;
	lMax = AR_MAX(af);
	if ( (lMax > thresh) && (gainTarget != MIN_GAIN) && (width > (HIST_SIZE>>2)) && (cnt > 10 * INTERVAL) ) {
		printf("%6.4f\n", lMax);
		gainTarget -= 2 << (int)lMax; cnt = 0;
		if (gainTarget < MIN_GAIN)
			gainTarget = MIN_GAIN;
	} else if ( (lMax <= thresh || width < (HIST_SIZE>>2)) && (cnt > HOLD_TIME) ) {
		gainTarget += 10; cnt = 0;
		if  (gainTarget > MAX_GAIN)
			gainTarget = MAX_GAIN;
	}
//	printf("%d\n", gainTarget);
}

// MCP4822
//#include <pigpio.h>
unsigned char spiBuf[2] = {};
#define SPEED 20000000

int main(int argc, char ** argv) {
	// INIT DATA STRUCTURES
	int fdR, fdW, ret, val, bot, top, mid, tmp;
	struct Queue * q = createQueue(MAX_SAMPLES+10); // leave a little extra space	

	// LED stuff
	int pin = 6;
	if (wiringPiSetupGpio() == -1)
		return 1;
	pinMode(pin, OUTPUT);

	// MCP4822 Stuff
	//if (gpioInitialise() < 0) return 1;
	//int h = spiOpen(1, SPEED, 0); // channel 1, speed, no flags
//	if (wiringPiSPISetup(0, SPEED) == -1) return 1;


	// READ
	int shift = (int) log2(RD_RANGE/HIST_SIZE);
	printf("%d\n", shift);
	READ_BUF_TYPE readBuf[READ_BUF_SIZE] = {};
	WRITE_BUF_TYPE writeBuf[WRITE_BUF_SIZE] = {};

	fdR = open(readFifo, O_RDONLY);
	if (fdR == -1) {printf("couldn't open fifo\n"); return 1;}
	
	fdW = open(writeFifo, O_WRONLY);	
	if (fdW == -1) {printf("couldn't open write fifo\n"); return 1;}
	
	while (1) {	
	ret = read(fdR, readBuf, READ_BUF_SIZE*sizeof(READ_BUF_TYPE));
	if (ret == -1)
		printf("read failed\n");
	else if (ret == 0) { printf("end of file\n"); return 0; }
	else {
		val = readBuf[0] >> shift;
		
	// UPDATE
		a[val] += 1;
		if ( val > max ) { 
			max = val;
//			printf("max now %d\n", max);
		}
		else if ( val < min) {
			min = val;
//			printf("min now %d\n", min);
		}
		enqueue(q, val);
		if ( q->size > MAX_SAMPLES ) {
			tmp = dequeue(q);
			a[tmp] -= 1;
			if (!a[tmp]) { // this value is now zero
				if (tmp == min) { 
					FIND_NEW_MIN(a, tmp); 
					//printf("old min: %d, new min: %d\n", tmp, min); 
				} else if (tmp == max) { 
					FIND_NEW_MAX(a, tmp); 
					//printf("old max: %d, new max: %d\n", tmp, max); 
				}
			}
//		}

	// CHECK
		//printf("%d\n", cnt);
			width = max - min;
			middle = ( max + min ) >> 1;
			if ( !(cnt++ % INTERVAL) ) {
				update();
				writeBuf[0] = gainTarget;
				write(fdW, writeBuf, WRITE_BUF_SIZE*sizeof(WRITE_BUF_TYPE));
			}
/*		
	// assert (min - 5 > 0)
//		printf("width: %d, comp: %d\n", width, HIST_SIZE>>2);
		// if ( width > ( HIST_SIZE >> 1 ) ) { // only if the histogram is wide enough
		float br, tr;
		if ( mid ) { // range of br and tr is ~ 0 to MAX_SAMPLES/2
			br = (float) bot / (float) mid;
			tr = (float) top / (float) mid;
		}
		int gainTarget;
		printf("%5.4f, %5.4f\n", br, tr);
		if (br > (MAX_SAMPLES >> 2))
			gainTarget = 1500;
		else if (br > (MAX_SAMPLES >> 3)) 
			gainTarget = 1600;
		else if (br > (MAX_SAMPLES >> 4))
			gainTarget = 1700;

//		digitalWrite(pin, 1);
		if ( ( width > (HIST_SIZE >> 2) ) && ( mid < (bot >> 1) || mid < (top >> 1) ) ) { 
			writeBuf[0] = 0x3bff; //writeBuf[0] = 1; 
//			wiringPiSPIDataRW(1, spiBuf, 2);
			//spiWrite(h, spiBuf, 2); 
			digitalWrite(pin, 1); 
		}
		//	printf("d\n");
		else { 
			writeBuf[0] = 0x3fff;
			write(fdW, writeBuf, WRITE_BUF_SIZE*sizeof(WRITE_BUF_TYPE));
			//spiWrite(h, spiBuf, 2); 
//			wiringPiSPIDataRW(1, spiBuf, 2);
			digitalWrite(pin, 0); 
		}
//		printf("%d\n", writeBuf[0]);
		//	printf("n\n");
*/
		}
	}
	}
// close the pipe
	close(fdR);
	close(fdW);

// cleanup spi
//	spiClose(h);
//	gpioTerminate();

	return 0;
}
