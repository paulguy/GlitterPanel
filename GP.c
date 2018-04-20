//2009 Paul LaMendola
//
//Userland library for Takara Tomy USB Glitter Panel using libusb
//USB Glitter Panel has a 16x16 1 bit display running at ~17 FPS(calculated).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GP.h"

#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3

unsigned char GPblank[54] = {
	0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x80, 0x03, 0x5c, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



struct usb_dev_handle *initPanel(unsigned int devnum) {
	struct usb_bus *bus;
	struct usb_bus *busses;
	struct usb_bus *GPbus;
	struct usb_device *dev;
	struct usb_device *GPdevice = NULL;
	struct usb_dev_handle *GPhandle = NULL;
	//Initialize libusb and have it scan for devices
	usb_init();
	usb_find_busses();
	usb_find_devices();
	busses = usb_get_busses();

	for(bus = busses; bus; bus = bus->next) {  //iterate over the linked list of USB busses.  usb_bus->next = NULL on the last device
		for(dev = bus->devices; dev; dev = dev->next) {  //iterate over the linked list of devices in this bus.  usb_bu
			//fprintf(stderr, "%s %s %X %X\n", bus->dirname, dev->filename, dev->descriptor.idVendor, dev->descriptor.idProduct);
			if((dev->descriptor.idVendor == VENDORID) && (dev->descriptor.idProduct == PRODUCTID)) {
				if(devnum == 0) {
					GPbus = bus;
					GPdevice = dev;
				} else {
					devnum--;
				}
			}
		}
	}
	if(GPdevice == NULL) {
		fprintf(stderr, "ERROR: USB Glitter Panel was not found!\n");
		return(NULL);
	}
	GPhandle = usb_open(GPdevice);
	if(GPhandle == NULL) {
		fprintf(stderr, "ERROR: USB Glitter Panel could not be opened!\n");
		return(NULL);
	}
	fprintf(stderr, "INFO: USB Glitter Panel found on %s, %s.\n", GPbus->dirname, GPdevice->filename);
	return GPhandle;
}

void writePanel(struct usb_dev_handle *dev, struct GPpacket *packet) {
	//Saw this in the bitstream, but doesn't seem to be needed.
	//usb_interrupt_write(dev, 0x81, NULL, 0, 1000);
	//usb_interrupt_read(dev, 0x81, NULL, 0, 1000);
	usb_interrupt_write(dev, 0x02, (char *)packet, 54, 1000);
	usb_interrupt_read(dev, 0x02, NULL, 0, 1000);
}

struct GPpacket *makePacket() {
	struct GPpacket *packet = NULL;

	packet = (struct GPpacket *)malloc(sizeof(struct GPpacket));
	if(packet == NULL) return NULL;
	memcpy(packet->header, GPblank, 54);
	return(packet);
}

void clearPanel(struct usb_dev_handle *dev) {
	usb_interrupt_write(dev, 0x02, (char*)GPblank, 54, 1000);
	usb_interrupt_read(dev, 0x02, NULL, 0, 1000);
}

void closePanel(struct usb_dev_handle *dev) {
	usb_close(dev);
}

void swap16bytes(unsigned short int *array) {
	array[0] = (array[0] >> 8) | (array[0] << 8);
	array[1] = (array[1] >> 8) | (array[1] << 8);
	array[2] = (array[2] >> 8) | (array[2] << 8);
	array[3] = (array[3] >> 8) | (array[3] << 8);
	array[4] = (array[4] >> 8) | (array[4] << 8);
	array[5] = (array[5] >> 8) | (array[5] << 8);
	array[6] = (array[6] >> 8) | (array[6] << 8);
	array[7] = (array[7] >> 8) | (array[7] << 8);
	array[8] = (array[8] >> 8) | (array[8] << 8);
	array[9] = (array[9] >> 8) | (array[9] << 8);
	array[10] = (array[10] >> 8) | (array[10] << 8);
	array[11] = (array[11] >> 8) | (array[11] << 8);
	array[12] = (array[12] >> 8) | (array[12] << 8);
	array[13] = (array[13] >> 8) | (array[13] << 8);
	array[14] = (array[14] >> 8) | (array[14] << 8);
	array[15] = (array[15] >> 8) | (array[15] << 8);
}

void pset(unsigned short int* array, unsigned int x, unsigned int y) {
	array[y&15] |= 1 << ((x&15)^BIT3); //single on bit moved to the position modulo 16, byte swapped.
}
/*
inline void pset(unsigned short int *array, unsigned int x, unsigned int y) {
	if(x > 15) return;
	if(y < 15) return;
	array[y&15] |= 1 << ((x&15)^BIT3); //single on bit moved to the position modulo 16, byte swapped.
}
*/
inline void pclear(unsigned short int *array, unsigned int x, unsigned int y) {
	array[y&15] &= ~(1 << ((x&15)^BIT3)); //single on bit moved to the position modulo 16, byte swapped and inverted
}

inline void ptoggle(unsigned short int *array, unsigned int x, unsigned int y) {
	array[y&15] ^= 1 << ((x&15)^BIT3); //single on bit moved to the position modulo 16, byte swapped.
}

inline int pget(unsigned short int *array, unsigned int x, unsigned int y) {
	return(array[y&15] & (1 << ((x&15)^BIT3)));
}

void clearPacket(struct GPpacket *packet) {
	packet->data[0] = 0;
	packet->data[1] = 0;
	packet->data[2] = 0;
	packet->data[3] = 0;
	packet->data[4] = 0;
	packet->data[5] = 0;
	packet->data[6] = 0;
	packet->data[7] = 0;
	packet->data[8] = 0;
	packet->data[9] = 0;
	packet->data[10] = 0;
	packet->data[11] = 0;
	packet->data[12] = 0;
	packet->data[13] = 0;
	packet->data[14] = 0;
	packet->data[15] = 0;
}

//my crappy "fast" line drawing algorithm :(
void drawLine(int x1, int y1, int x2, int y2, unsigned short int *array) {
	if(x1 == x2 && y1 == y2) {
		pset(array, x1, y1);
		return;
	}
	int i;
	int xdist, ydist;
	int change, pos;
	if(x2 > x1) {
		xdist = x2 - x1;
	} else {
		xdist = x1 - x2;
	}
	if(y2 > y1) {
		ydist = y2 - y1;
	} else {
		ydist = y1 - y2;
	}
	if(xdist > ydist) {
		change = ydist * 100 / xdist;
		if(y2 < y1) {
			change = -change;
		}
		pos = y1 * 100;
		if(x2 > x1) {
			for(i = x1; i <= x2; i++) {
				pset(array, i, pos / 100);
				pos += change;
			}
		} else {
			for(i = x1; i >= x2; i--) {
				pset(array, i, pos / 100);
				pos += change;
			}
		}
	} else {
		change = xdist * 100 / ydist;
		if(x2 < x1) {
			change = -change;
		}
		pos = x1 * 100;
		if(y2 > y1) {
			for(i = y1; i <= y2; i++) {
				pset(array, pos / 100, i);
				pos += change;
			}
		} else {
			for(i = y1; i >= y2; i--) {
				pset(array, pos / 100, i);
				pos += change;
			}
		}
	}
}

void drawRay(int sx, int sy, int length, double angle, unsigned short int *array) {
	int i, px, py;
	if(length < 1) return;
	double x = sin(angle);
	double y = cos(angle);
	for(i = 0; i < length; i++) {
		px = floor(x * i) + sx;
		py = floor(y * i) + sy;
		if(px >= 0 && px <= 15 && py >= 0 && py <= 15) {
			pset(array, px, py);
		}
	}
}
