#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <termios.h>
#include <sys/time.h>

#include "ttyusb.h"

#define BUFFER_SIZE 256
#define TTY_TIMEOUT 10000

int main(int args,char *argv[])
{
  ttyusb *psttty = (ttyusb*)malloc(sizeof(ttyusb));
  int buadrate=0;
  
  if(psttty==0){
    printf("[%s,%d]fail to malloc ttyusb structure.\n",__FILE__,__LINE__);
    return -1;
  }
  if(args<4){
    printf("usage: serial_tx [/dev/ttyUSBx] [buadrate] [string]\n"
           "\teg:serial_tx /dev/ttyUSB0 115200 \"hello world\" \n");
    goto exit;
  }
  buadrate = atoi(argv[2]);//buadrate

  if(init_tty(psttty,argv[1],buadrate)==FALSE){
    printf("[%s,%d]fail to initialize the ttyusb device.\n",__FILE__,__LINE__);
    return -1;
  }else{
    printf("Successfully initialized the tty device: %s at %d buadrate.\n",argv[1],buadrate);
  }

  send_data(psttty->fd, (unsigned char *)(argv[3]),strlen(argv[3]));
exit:
  if(psttty)free(psttty);
  return 0;
}
