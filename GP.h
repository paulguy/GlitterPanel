#ifndef GP_H
#define GP_H

#include <usb.h>

#define BIT0 (1)
#define BIT1 (2)
#define BIT2 (4)
#define BIT3 (8)
#define BIT4 (16)
#define BIT5 (32)
#define BIT6 (64)
#define BIT7 (128)

#define VENDORID (0x0F30) //Takara Tomy (reported as Jess Technologies Co., Ltd)
#define PRODUCTID (0x0040) //USB Glitter Panel
struct GPpacket {
	char header[22];
	unsigned short int data[16];
};

struct usb_dev_handle *initPanel(unsigned int devnum);
void writePanel(struct usb_dev_handle *dev, struct GPpacket *packet);
struct GPpacket *makePacket();
void clearPanel(struct usb_dev_handle *dev);
void closePanel(struct usb_dev_handle *dev);
void swap16bytes(unsigned short int *array);
inline void pset(unsigned short int *array, unsigned int x, unsigned int y);
inline void pclear(unsigned short int *array, unsigned int x, unsigned int y);
inline void ptoggle(unsigned short int *array, unsigned int x, unsigned int y);
inline int pget(unsigned short int *array, unsigned int x, unsigned int y);
inline void clearPacket(struct GPpacket *packet);
void drawLine(int x1, int y1, int x2, int y2, unsigned short int *array);
void drawRay(int x, int y, int length, double angle, unsigned short int *array);

#endif
