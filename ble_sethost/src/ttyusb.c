#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>

#include "ttyusb.h"


static int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	//get current serial setting.
	if ( tcgetattr( fd,&oldtio) != 0) 
	{ 
		return -1;
	}

	memset( &newtio,0, sizeof( newtio ) );
	
	newtio.c_cflag |= CLOCAL | CREAD; 
	newtio.c_cflag &= ~CSIZE;

	switch( nBits )
	{
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}

	switch( nEvent )
	{
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E': 
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N': 
		newtio.c_cflag &= ~PARENB;
		break;
	}
	switch( nSpeed )
	{
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
  case 19200:
    cfsetispeed(&newtio, B19200);
		cfsetospeed(&newtio, B19200);
		break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}

	//set stop bit
	if( nStop == 1 )
		newtio.c_cflag &= ~CSTOPB;
	else if ( nStop == 2 )
		newtio.c_cflag |= CSTOPB;


	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;
	tcflush(fd,TCIFLUSH);

	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
	return -1;
	}
	return 0;
}

BOOL init_tty(ttyusb * pstusb, const char * devpath, int buadrate)
{
	struct termios newtio,oldtio;
	int fdtty;
	BOOL ret = TRUE;

	if(pstusb==NULL || devpath==NULL || buadrate<=0 )return FALSE;

	//1.open tty device.	
	fdtty = open(devpath,O_RDWR | O_NOCTTY | O_NDELAY);
	if(-1==fdtty){
		printf("[%s,%d]fuck!no device found\n",__FILE__,__LINE__);
		return FALSE;
	}	

	//2.set port.
	if(set_opt(fdtty,buadrate,8,'N',1)<0)
	{
		printf("[%s,%d]fuck!no device found\n",__FILE__,__LINE__);
		return FALSE;
	}

	//update the struct
	memset(pstusb,0,sizeof(ttyusb));
	pstusb->fd = fdtty;
	strcpy(pstusb->devpath,devpath);
	ret = TRUE;
	return TRUE;
	
exit:
	close(fdtty);
	return FALSE;
}

int send_data(int fdtty, unsigned char *data,int len)
{
	int ret=0;
	struct timeval timeout;
	if(fdtty<0)return -1;
	
	ret = write(fdtty,(char*)data,len);//写数据    
	if(ret==0)return -1;
	
	timeout.tv_sec=0;
	timeout.tv_usec=20000;//设置时间 20MS 读取一下串口数据
	ret=select(0,NULL,NULL,NULL,&timeout);

	return ret;
}
