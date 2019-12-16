#include <stdio.h>
#include <wiringPiSPI.h>

int main(int argc, char **argv) {
	int ret = wiringPiSPISetup(0, 1000000);
	//printf("%i\n", ret);

	return 0;
}
