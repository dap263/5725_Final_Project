/*
Modified by: David Pirogovsky <dap263@cornell.edu> in Nov. 2019

CODE BASED OFF OF EXAMPLE CODE FROM PIGPIO DOCUMENTATION:
http://abyz.me.uk/rpi/pigpio/examples.html
rawMCP3008.c
Public Domain
2016-03-20

gcc -Wall -pthread -o rawMCP3008 rawMCP3008.c -lpigpio

This code shows how to bit bang SPI using DMA.

Using DMA to bit bang allows for two advantages

1) the time of the SPI transaction can be guaranteed to within a
   microsecond or so.

2) multiple devices of the same type can be read or written
  simultaneously.

This code shows how to read more than one MCP3008 at a time.

Each MCP3008 shares the SPI clock, MOSI, and slave select lines but has
a unique MISO line.

This code should also work with the MCP3004.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pigpio.h>

#define SPI_SS 26 // GPIO for slave select.

#define ADCS 1    // Number of connected ADC

#define BITS 8            // Bits per reading.
#define BX 8               // Bit position of data bit B7.
#define B0 (BX + BITS - 1) // Bit position of data bit B0.

#define MISO1 13  // ADC 1 MISO.

#define BUFFER 250       // Generally make this buffer as large as possible.

#define REPEAT_MICROS 40 // Reading every x microseconds.

//#define SAMPLES 2000000  // Number of samples to take,
#define SAMPLES 10000

//int MISO[ADCS]= MISO1;
int MISO = MISO1;

rawSPI_t rawSPI =
{
   .clk     =  5, // GPIO for SPI clock.
   .mosi    = 12, // GPIO for SPI MOSI.
   .ss_pol  =  1, // Slave select resting level.
   .ss_us   =  5, // Wait 1 micro after asserting slave select.
   .clk_pol =  0, // Clock resting level.
   .clk_pha =  0, // 0 sample on first edge, 1 sample on second edge.
   .clk_us  =  1, // 2 clocks needed per bit so 500 kbps.
};

/*
   This function extracts the MISO bits for each ADC and
   collates them into a reading per ADC.
*/

void getReading(
   int adcs,  // Number of attached ADCs.
   int MISO,  // The GPIO connected to the ADCs data out.
   int OOL,   // Address of first OOL for this reading.
   int bytes, // Bytes between readings.
   int bits,  // Bits per reading.
   char *buf) 
{
   int i, p;
   uint32_t level;

   p = OOL;

   for (i=0; i<bits; i++)
   {
      level = rawWaveGetOut(p);
	  printf(" %d ", level);
      putBitInBytes(i, buf+(bytes), level & (1<<MISO));
      

      p--;
   }
}


int main(int argc, char *argv[])
{
   int i, wid, offset;
   char buf[2];
   gpioPulse_t final[2];
   char rx[8];
   int sample;
   int val;
   int cb, botCB, topOOL, reading, now_reading;
   float cbs_per_reading;
   rawWaveInfo_t rwi;
   double start, end;
   int pause;

   if (argc > 1) pause = atoi(argv[1]); else pause =0;

   if (gpioInitialise() < 0) return 1;

   // Need to set GPIO as outputs otherwise wave will have no effect.

   gpioSetMode(rawSPI.clk,  PI_OUTPUT);
   gpioSetMode(rawSPI.mosi, PI_OUTPUT);
   gpioSetMode(SPI_SS,      PI_OUTPUT);

   gpioWaveAddNew(); // Flush any old unused wave data.

   offset = 0;

    /*
    ADC0832CCN 2 8-bit channels
	may be possible to send start bit whenever
    1  2  3  4  5  6    7    8     9  10 11 12 13 14 15 16
    0  0  0  0  SB mode chan 0     B7 B6 B5 B4 B3 B2 B1 B0
    SB      1 // start bit
    mode    0=differential, 1=single
    chan    0 or 1
    */ 

   /*
   MCP3004/8 10-bit ADC 4/8 channels

   1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17
   SB SD D2 D1 D0 NA NA B9 B8 B7 B6 B5 B4 B3 B2 B1 B0

   SB       1
   SD       0=differential 1=single
   D2/D1/D0 0-7 channel
   */

   /*
      Now construct lots of bit banged SPI reads.  Each ADC reading
      will be stored separately.  We need to ensure that the
      buffer is big enough to cope with any reasonable rescehdule.

      In practice make the buffer as big as you can.
   */

   for (i=0; i<BUFFER; i++)
   {
      // 0b 1100 0000
      buf[0] = 0xC0; // Start bit, single ended, channel 0.

    // (rawSPI_t *spi, unsigned offset, unsigned spiSS, char *buf, unsigned spiTxBits, unsigned spiBitFirst, unsigned spiBitLast, unsigned spiBits)
      rawWaveAddSPI(&rawSPI, offset, SPI_SS, buf, 3, BX, B0, B0); // TODO: should # be 2 or 3? or 8? 

      /*
         REPEAT_MICROS must be more than the time taken to
         transmit the SPI message.
      */

      offset += REPEAT_MICROS;
   }


// TODO: is this final stuff necessary? 
   // Force the same delay after the last reading.

   final[0].gpioOn = 0;
   final[0].gpioOff = 0;
   final[0].usDelay = offset;

   final[1].gpioOn = 0; // Need a dummy to force the final delay.
   final[1].gpioOff = 0;
   final[1].usDelay = 0;

   gpioWaveAddGeneric(2, final);

   wid = gpioWaveCreate(); // Create the wave from added data.

   if (wid < 0)
   {
      fprintf(stderr, "Can't create wave, %d too many?\n", BUFFER);
      return 1;
   }

   /*
      The wave resources are now assigned,  Get the number
      of control blocks (CBs) so we can calculate which reading
      is current when the program is running.
   */

   rwi = rawWaveInfo(wid);

   printf("# cb %d-%d ool %d-%d del=%d ncb=%d nb=%d nt=%d\n",
      rwi.botCB, rwi.topCB, rwi.botOOL, rwi.topOOL, rwi.deleted,
      rwi.numCB,  rwi.numBOOL,  rwi.numTOOL);

   /*
      CBs are allocated from the bottom up.  As the wave is being
      transmitted the current CB will be between botCB and topCB
      inclusive.
   */

   botCB = rwi.botCB;

   /*
      Assume each reading uses the same number of CBs (which is
      true in this particular example).
   */

   cbs_per_reading = (float)rwi.numCB / (float)BUFFER;

   printf("# cbs=%d per read=%.1f base=%d\n",
      rwi.numCB, cbs_per_reading, botCB);

   /*
      OOL are allocated from the top down. There are BITS bits
      for each ADC reading and BUFFER ADC readings.  The readings
      will be stored in topOOL - 1 to topOOL - (BITS * BUFFER).
   */

   topOOL = rwi.topOOL;

   fprintf(stderr, "starting...\n");

   if (pause) time_sleep(pause); // Give time to start a monitor.

   gpioWaveTxSend(wid, PI_WAVE_MODE_REPEAT);

   reading = 0;

   sample = 0;

   start = time_time();

   while (sample<SAMPLES)
   {
      // Which reading is current?

      cb = rawWaveCB() - botCB;

      now_reading = (float) cb / cbs_per_reading;

      // Loop gettting the fresh readings.

      while (now_reading != reading)
      {
         /*
            Each reading uses BITS OOL.  The position of this readings
            OOL are calculated relative to the waves top OOL.
         */
        // (int adcs,  int MISO, int OOL, int bytes, int bits, char *buf)
         getReading(
            ADCS, MISO, topOOL - ((reading%BUFFER)*BITS) - 1, 1, BITS, rx);

         printf("%d", ++sample);

         for (i=0; i<ADCS; i++)
         {
            //  7  6  5  4  3  2  1  0 |  7  6  5  4  3  2  1  0
            // B9 B8 B7 B6 B5 B4 B3 B2 | B1 B0  X  X  X  X  X  X

            val = rx[i];
            printf(" %d", val);
         }

         printf("\n");

         if (++reading >= BUFFER) reading = 0;
      }
      usleep(1000);
   }

   end = time_time();

   printf("# %d samples in %.1f seconds (%.0f/s)\n",
      SAMPLES, end-start, (float)SAMPLES/(end-start));

   fprintf(stderr, "ending...\n");

   if (pause) time_sleep(pause);

   gpioTerminate();

   return 0;
}

