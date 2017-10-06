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
#define ADS1000_ADDR 0x48
int main(int args,char* argv[])
{
  int i2c_fd = i2c_openadapter(I2C_DEV);
  if(i2c_fd<0){
    printf("[%1,%2]fail to open i2c_dev\n",__FILE__,__LINE__);
    return -1;
  }

  //step1. open slave addr
  unsigned char addr = ADS1000_ADDR;
  printf("reading i2c addr:0x%x\n",addr);
  i2c_setslave(i2c_fd,addr);

  //step2. read i2c_addr
  const int rd_size=3;
  uint8_t rd_buffer[rd_size];
  uint16_t volt_raw;

  for(int i=0;i<300;++i){
    int len = i2c_readbytes(i2c_fd,rd_buffer,rd_size);
    printf("rx_len=%d:",len);
    
#if 0 
    for(int k=0;k<len;++k){
      printf("0x%x ",rd_buffer[k]);
    }
    printf("\n");
#endif

    //step1.convert raw.
    uint8_t* ptr_data = (uint8_t*)&volt_raw;
    ptr_data[1] = rd_buffer[0];
    ptr_data[0] = rd_buffer[1];
    printf("volt_raw:0x%x,",volt_raw);

    //step2.convert to voltage:
    float p_v = static_cast<float>(volt_raw)/2048.0f;
    float voltage = p_v*6.6f;
    printf("p:%.2f, voltage:%.2f\n",p_v,voltage);
    sleep(1);
  }
  return 0;
}
