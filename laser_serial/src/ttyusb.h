#ifndef TTYUSB_H
#define TTYUSB_H

#include <stdlib.h>

#define DATABUFFER 256
typedef struct ttyusb_s{
	int fd;
	char devpath[16];
	unsigned char txbuffer[DATABUFFER];
	unsigned char rxbuffer[DATABUFFER];
}ttyusb;

#define BOOL int
#define TRUE  0
#define FALSE 1
/******************************************************
*purpose:	init the tty usb device. 
*input:		pstusb, the usb device structure.
*			devpath, the tty usb device dev file name
*			buadrate, the buadrate of the serial com
*return:		true, if success.
*******************************************************/
BOOL init_tty(ttyusb * pstusb, const char * devpath,int buadrate);

int send_data(int fdtty, unsigned char *data,int len);


#endif
