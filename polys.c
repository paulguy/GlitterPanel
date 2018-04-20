#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "GP.h"

#define POINTS (5)
#if POINTS < 2
	#error POINTS must be >= 2
#endif

void cleanup(int param);

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

	unsigned int i;
	struct usb_dev_handle *GPhandle;
	struct GPpacket *screen;
	double *x, *y;
	double *vx, *vy;
	int running = 1;

	GPhandle = initPanel(0);
	if(GPhandle == NULL) {
		fprintf(stderr, "ERROR: Couldn't initialize USB Glitter Panel!\n");
		return(EXIT_FAILURE);
	}
	screen = makePacket();
	if(screen == NULL) {
		closePanel(GPhandle);
		return(EXIT_FAILURE);
	}

	srand(time(NULL));
	x = (double *)malloc(sizeof(double) * POINTS);
	y = (double *)malloc(sizeof(double) * POINTS);
	vx = (double *)malloc(sizeof(double) * POINTS);
	vy = (double *)malloc(sizeof(double) * POINTS);
	for(i = 0; i < POINTS; i++) {
		x[i] = (double)rand() / RAND_MAX * 15;
		y[i] = (double)rand() / RAND_MAX * 15;
		vx[i] = (double)rand() / RAND_MAX * 2 - 1;
		vy[i] = (double)rand() / RAND_MAX * 2 - 1;
	}

	while(running) {
		clearPacket(screen);
		for(i = 0; i < POINTS - 1; i++) {
			drawLine(x[i], y[i], x[i + 1], y[i + 1], screen->data);
		}
		for(i = 0; i < POINTS; i++) {
//			fprintf(stderr, "%f %f\n", x[i], y[i]);
			x[i] += vx[i];
			if(x[i] < 0) {
				x[i] = 0;
				vx[i] = -vx[i];
				vy[i] = (double)rand() / RAND_MAX * 2 - 1;
			}
			if(x[i] > 15) {
				x[i] = 15;
				vx[i] = -vx[i];
				vy[i] = (double)rand() / RAND_MAX * 2 - 1;
			}
			y[i] += vy[i];
			if(y[i] < 0) {
				y[i] = 0;
				vy[i] = -vy[i];
				vx[i] = (double)rand() / RAND_MAX * 2 - 1;
			}
			if(y[i] > 15) {
				y[i] = 15;
				vy[i] = -vy[i];
				vx[i] = (double)rand() / RAND_MAX * 2 - 1;
			}
		}
		writePanel(GPhandle, screen);
	}

	clearPanel(GPhandle);
	closePanel(GPhandle);
	fprintf(stderr, "Done.\n");
	return(EXIT_SUCCESS);
}

