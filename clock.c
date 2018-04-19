#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "GP.h"
#define PISIXTY 0.10472
#define PITWOFOUR 0.2618

void cleanup(int param);

int main(int argc, char **argv) {
	unsigned int i, running = 1;
	double x, y;
	struct usb_dev_handle *GPhandle;
	struct GPpacket *screen;
	time_t unixtime;
	struct tm *thistime;

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

	while(running) {
		clearPacket(screen);
		time(&unixtime);
		thistime = localtime(&unixtime);
		pset(screen->data,
			7 - floor(sin((double)(thistime->tm_sec) * PISIXTY) * 8),
			7 - floor(cos((double)(thistime->tm_sec) * PISIXTY) * 8));
		x = sin((double)(thistime->tm_min) * PISIXTY);
		y = cos((double)(thistime->tm_min) * PISIXTY);
		for(i = 0; i < 8; i++) {
			pset(screen->data, 7 - floor(x * i), 7 - floor(y * i));
		}
		x = sin((double)(thistime->tm_hour) * PITWOFOUR);
		y = cos((double)(thistime->tm_hour) * PITWOFOUR);
		for(i = 0; i < 6; i++) {
			pset(screen->data, 7 - floor(x * i), 7 - floor(y * i));
		}
		writePanel(GPhandle, screen);
	}

	clearPanel(GPhandle);
	closePanel(GPhandle);
	fprintf(stderr, "Done.\n");
	return(EXIT_SUCCESS);
}

