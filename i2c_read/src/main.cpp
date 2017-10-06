#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "i2c.h"
using namespace std;

#define I2C_DEV "/dev/i2c-0"

int main(int args,char* argv[])
{
  if(args<2){
    printf("usage: i2c_read [addr]");
    return -1;
  }

  int i2c_fd = i2c_openadapter(I2C_DEV);
  if(i2c_fd<0){
    printf("[%1,%2]fail to open i2c_dev\n",__FILE__,__LINE__);
    return -1;
  }

  unsigned char addr = (unsigned char)atol(argv[1]);
  printf("reading i2c addr:0x%x\n",addr);
  i2c_setslave(i2c_fd,addr);

  const int rd_size=5;
  uint8_t rd_buffer[rd_size];
  for(int i=0;i<5;++i){
    int len = i2c_readbytes(i2c_fd,rd_buffer,rd_size);
    printf("rx_len=%d:",len);
    sleep(1);
    for(int k=0;k<len;++k){
      printf("0x%x ",rd_buffer[k]);
    }
    printf("\n");
  }
  return 0;
}
