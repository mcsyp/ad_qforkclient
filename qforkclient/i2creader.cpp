#include "i2creader.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include "i2c-dev-user.h"
#include <qdebug.h>

I2CReader::I2CReader(QObject *parent){
  i2c_fd_ = -1;
  slave_addr_ = 0;
}

bool I2CReader::Begin(const char *dev_path)
{
  if(dev_path==NULL)return -1;
  if ((i2c_fd_ = open(dev_path, O_RDWR)) < 1) {
    qDebug()<<tr("[%1,%2]fail to open i2c device:%3").arg(__FILE__).arg(__LINE__).arg(dev_path);
    return false;
  }
  return true;
}

bool I2CReader::SetSlave(uint8_t addr){
  if(i2c_fd_<0)return false;
  if (ioctl(i2c_fd_, I2C_SLAVE_FORCE, addr) < 0) {
    qDebug()<<tr("[%1,%2]fail to set %3's slave addr:%4")
              .arg(__FILE__).arg(__LINE__)
              .arg(i2c_fd_)
              .arg(addr);
    return false;
  }
  slave_addr_ = addr;
  return true;
}

void I2CReader::WriteByte(uint8_t b){
  if(i2c_fd_<0)return;
  if (i2c_smbus_write_byte(i2c_fd_, b) < 0) {
    qDebug()<<tr("[%1,%2]fail to set %3's to value:%4")
              .arg(__FILE__).arg(__LINE__)
              .arg(i2c_fd_)
              .arg(b);
  }
}
void I2CReader::WriteBytes(uint8_t buffer[], int len){
  if(i2c_fd_<0)return;
  if(i2c_smbus_write_i2c_block_data(i2c_fd_, buffer[0], len-1, buffer+1) < 0) {
    qDebug()<<tr("[%1,%2]fail to write bytes[%3] to %4")
              .arg(__FILE__).arg(__LINE__)
              .arg(len)
              .arg(i2c_fd_);
  }
}
int I2CReader::ReadByte(){
  int byte;
  if(i2c_fd_<0)return -1;
  if ((byte = i2c_smbus_read_byte(i2c_fd_)) < 0) {
    qDebug()<<tr("[%1,%2]fail to read byte from %3")
              .arg(__FILE__).arg(__LINE__)
              .arg(i2c_fd_);
    return -1;
  }
  return byte;
}
int I2CReader::ReadBytes(uint8_t out[], int out_size){
  if(i2c_fd_<0)return -1;
  if (read(i2c_fd_, out, out_size) == out_size)
    return out_size;
  else
    return -1;
}
