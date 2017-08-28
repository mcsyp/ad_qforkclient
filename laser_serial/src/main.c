#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <termios.h>
#include <sys/time.h>
#include "ttyusb.h"

#define TTYDEV "/dev/ttyUSB0"
#define BUFFER_SIZE 256
#define BUADRATE 19200
#define TTY_TIMEOUT 10000

void  handle_rx_message(char rx_buffer[],int rx_len){
  char* offset_start = strchr(rx_buffer,':');
  if(offset_start==NULL)return;

  char* offset_end = strchr(rx_buffer, 'm');
  if(offset_end==NULL)return;

  int str_start = offset_start-rx_buffer+1;
  offset_end[0]='\0';
  float val = atof(rx_buffer+str_start);
  printf("distance:%f\n",val);
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

  if(psttty==0){
    printf("[%s,%d]fail to malloc ttyusb structure.\n",__FILE__,__LINE__);
    return -1;
  }
  if(args<2){
    printf("usage: mavflow [/dev/ttyUSBx]\n"
           "\teg:mavflow /dev/ttyUSB0\n");
    goto exit;
  }

  if(init_tty(psttty,argv[1],BUADRATE)==FALSE){
    printf("[%s,%d]fail to initialize the ttyusb device.\n",__FILE__,__LINE__);
    return -1;
  }else{
    printf("Successfully initialized the tty device: %s at %d buadrate.\n",argv[1],BUADRATE);
  }

  if(pthread_create(&rx_thread,NULL,rx_serial,psttty)){
    printf("[%s,%d]fail to create the tty rx thread.\n",__FILE__,__LINE__);
    goto exit;
  }


  //turn on the laser
  send_data(psttty->fd,"O",1);
  sleep(3);
  while(1){
    printf("trigger laser\n");
    send_data(psttty->fd,"D",1);
    sleep(1);
  }
exit:
  if(psttty)free(psttty);
  return 0;
}
