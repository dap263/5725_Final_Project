#include <stdio.h>
#include <stdlib.h>

#include <pigpio.h>

/*
   gcc -pthread -o spi MCP4822.c -lpigpio

   sudo ./spi [bytes [bps [loops] ] ]
*/

#define LOOPS 10000
#define SPEED 1000000
#define BYTES 2

int main(int argc, char *argv[])
{
   int loops=LOOPS;
   int speed=SPEED;
   int bytes=BYTES;
   int i;
   int h;
   double start, diff;
   char buf[2];

	int level;
   if (argc > 1) level = atoi(argv[1]);
	if (level < 10 || level > 4095) level = 10;
  // else printf("sudo ./spi-pigpio-speed [bytes [bps [loops] ] ]\n\n");

   if ((bytes < 1) || (bytes > 16383)) bytes = BYTES;

   if (argc > 2) speed = atoi(argv[2]);
   if ((speed < 32000) || (speed > 250000000)) speed = SPEED;

   if (argc > 3) loops = atoi(argv[3]);
   if ((loops < 1) || (loops > 10000000)) loops = LOOPS;

   if (gpioInitialise() < 0) return 1;

   h = spiOpen(1, speed, 0);

   if (h < 0) return 2;

   start = time_time();

	buf[0] = 0x30 | (level >> 8);
	buf[1] = level & 0x00ff;

//   for (i=0; i<loops; i++)
//   {
      spiWrite(h, buf, bytes);
//   }

   diff = time_time() - start;

   printf("sps=%.1f: %d bytes @ %d bps (loops=%d time=%.1f)\n",
      (double)loops / diff, bytes, speed, loops, diff);

   spiClose(h);

   gpioTerminate();

   return 0;
}

