#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define WRITE_BUF_SIZE 1
#define WRITE_BUF_TYPE int

char *path = "/home/dap263/ECE-5725/Final-Project/adcBuf";

int main(int argc, char **argv) {
	int fd = open(path, O_WRONLY);
	FILE *fp = fdopen(fd, "w");
	WRITE_BUF_TYPE writeBuf[WRITE_BUF_SIZE] = {};
	long count = 0;
	int i, val, size;
	size = 256; 
	sleep(1);
	while (1) {
		for (i; i < size; i++) {
			val = 511*(sin(i*6.283/size) + 1);
			writeBuf[0] = val;
//			writeBuf[1] = '\0';
			write(fd, (void *) writeBuf, WRITE_BUF_SIZE*sizeof(WRITE_BUF_TYPE));
			//fprintf(fp, "%04d\n", val); 
			count+=1;
			usleep(50);
		}
		printf("%ld\n", count);fflush(stdout);
	}
	return 0;
}
