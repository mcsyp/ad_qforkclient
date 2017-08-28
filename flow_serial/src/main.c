#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <termios.h>
#include <sys/time.h>

#include "ttyusb.h"
#include "common/mavlink.h"

#define TTYDEV "/dev/ttyUSB0"
#define BUADRATE 115200
#define BUFFER_SIZE 256

#define GROUND_DISTANCE 0.6f
#define FOCAL_LENGTH_PIX 166.6666f
#define PIXFLOW_SCALE 10.0f

mavlink_optical_flow_t flow_;
mavlink_optical_flow_rad_t flow_rad_;
void  handle_rx_message(char rx_buffer[],int rx_len){
  static mavlink_message_t msg;
  static mavlink_status_t  status = {0};
  static float accumulated_dist_x=0.0f;
  static float accumulated_dist_y=0.0f;
  float  dist_x=0.0f,dist_y=0.0f;
  for(int i=0;i<rx_len;++i){
    if(mavlink_parse_char(MAVLINK_COMM_0,rx_buffer[i],&msg,&status)){
      /*rx a whole mavlink message*/
      switch(msg.msgid){
      case MAVLINK_MSG_ID_OPTICAL_FLOW:
        mavlink_msg_optical_flow_decode(&msg,&flow_);
        if((flow_.flow_x || flow_.flow_y)&&flow_.quality>100){
          printf("[%u] quality:%d flow:%d,%d\n",(unsigned int)flow_.time_usec,flow_.quality,flow_.flow_x,flow_.flow_y);
          dist_x = (flow_.flow_x/PIXFLOW_SCALE)/FOCAL_LENGTH_PIX*GROUND_DISTANCE;
          dist_y = (flow_.flow_y/PIXFLOW_SCALE)/FOCAL_LENGTH_PIX*GROUND_DISTANCE;
          printf("\tdelta distance:%f,%f\n",dist_x,dist_y);
          accumulated_dist_x += dist_x;
          accumulated_dist_y += dist_y;
          printf("\taccumulated distance:%f,%f\n",accumulated_dist_x,accumulated_dist_y);
        }
        break;
      case MAVLINK_MSG_ID_OPTICAL_FLOW_RAD:
        mavlink_msg_optical_flow_rad_decode(&msg,&flow_rad_);
        break;
      }
    }
  }
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
      timeout.tv_usec=10000;
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
    printf("usage: flow_serial [/dev/ttyUSBx] \n"
           "\teg:flow_serial /dev/ttyUSB0\n");
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


  printf("[%s,%d] sizeof mavlink flow struct:%d\n",__FILE__,__LINE__,(int)sizeof(mavlink_optical_flow_t));
  printf("[%s,%d] sizeof mavlink flow rad struct:%d\n",
         __FILE__,__LINE__,
         (int)sizeof(mavlink_optical_flow_rad_t));
  while(1){
    sleep(20);
  }
exit:
  if(psttty)free(psttty);
  return 0;
}
