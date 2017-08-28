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

void  handle_rx_message(char rx_buffer[],int rx_len){
  printf("rx:%s\n",rx_buffer);
  printf("data:");
  for(int i=0;i<rx_len;++i){
    printf("0x%x ",rx_buffer[i]);
  }
  printf("\n\n");
}

void* rx_serial(void *pst)
{
  ttyusb* psttty = (ttyusb*)pst;
  int fdrx;
  fd_set fds;
  int nread;
  int receFlag=0;
  char buff[BUFFER_SIZE];
  struct timeval timeout;

  if(psttty==NULL){
    printf("[%s,%d] NULL pointer of pstty.\n",__FILE__,__LINE__);
    return NULL;
  }
  fdrx = psttty->fd;
  while (1)
  {
    nread = read(fdrx,buff,BUFFER_SIZE);
    if(nread>0)
    {
      receFlag=2;
      handle_rx_message(buff,nread);
    }
    else
    {
      FD_ZERO(&fds);
      FD_SET(fdrx,&fds);
      if(receFlag>1)receFlag--;
      if(receFlag==1)
      {
        receFlag=0;
      }
      timeout.tv_sec=0;
      timeout.tv_usec=TTY_TIMEOUT;
      select(fdrx+1,&fds,NULL,NULL,&timeout);
    }
  }
  return NULL;
}
int main(int args,char *argv[])
{
  ttyusb *psttty = (ttyusb*)malloc(sizeof(ttyusb));
  pthread_t rx_thread;
  int buadrate=0;
  

  if(psttty==0){
    printf("[%s,%d]fail to malloc ttyusb structure.\n",__FILE__,__LINE__);
    return -1;
  }
  if(args<3){
    printf("usage: serial_rx [/dev/ttyUSBx] [buadrate]\n"
           "\teg:serial_rx /dev/ttyUSB0 115200\n");
    goto exit;
  }
  buadrate = atoi(argv[2]);//buadrate

  if(init_tty(psttty,argv[1],buadrate)==FALSE){
    printf("[%s,%d]fail to initialize the ttyusb device.\n",__FILE__,__LINE__);
    return -1;
  }else{
    printf("Successfully initialized the tty device: %s at %d buadrate.\n",argv[1],buadrate);
  }

  if(pthread_create(&rx_thread,NULL,rx_serial,psttty)){
    printf("[%s,%d]fail to create the tty rx thread.\n",__FILE__,__LINE__);
    goto exit;
  }


  //turn on the laser
  while(1){
    sleep(3);
  }
exit:
  if(psttty)free(psttty);
  return 0;
}
