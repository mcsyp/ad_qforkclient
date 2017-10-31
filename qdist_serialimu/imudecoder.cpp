#include "imudecoder.h"
#include <string.h>
#include <qdebug.h>
ImuDecoder::ImuDecoder(QObject *parent) : QObject(parent){
  reset();
}

void ImuDecoder::reset(){
  memset(rx_queue_,0,MAX_PACK_SIZE);
  memset(imu_data_,0,sizeof(float)*IMU_DECODED_AXIS);
  rx_len_ = 0;
  rx_started_ = false;
}

void ImuDecoder::push(uint8_t b)
{
  //if rx the start code reset the queue
  if(b==START_CODE){
    reset();
    rx_started_ = true;
  }

  if(!rx_started_)return;


  //if already started
  //save the data to the pack.
  if(rx_len_<MAX_PACK_SIZE){
    rx_queue_[rx_len_] = b;
    ++rx_len_;
  }

  if(rx_len_>=MAX_PACK_SIZE){
    //if rx queue is full
    //step1. crc check
    uint8_t my_crc = rx_queue_[MAX_PACK_SIZE-1];
    uint8_t calc_crc = computeCRC(rx_queue_,MAX_PACK_SIZE-1);

    if(my_crc==calc_crc){
      //step1.if the pack is valid, decode rxbuffer
      int imu_len=this->decode(rx_queue_,rx_len_,imu_data_,IMU_DECODED_AXIS);

      //step2. emit signal
      if(imu_len>=IMU_DECODED_AXIS){
        this->pack_id_ = rx_queue_[1];
        emit readyRead();
      }
    }

    //step2. clear the queue
    reset();
  }
}

uint8_t ImuDecoder::computeCRC(uint8_t *bytes, int len)
{
  long tmp=0;
  for(int i=0;i<len;++i){
    tmp+=bytes[i];
  }
  return static_cast<uint8_t>(tmp);
}

int ImuDecoder::readData(float out_array[], int out_size)
{
  if(out_size<IMU_DECODED_AXIS || out_array==NULL)return 0;
  memcpy((uint8_t*)out_array,(uint8_t*)(this->imu_data_),sizeof(float)*IMU_DECODED_AXIS);
  return IMU_DECODED_AXIS;
}

int ImuDecoder::readID() const
{
  return this->pack_id_;
}

int ImuDecoder::decode(uint8_t *buffer, int len, float out_array[], int out_size)
{
  if(len>MAX_PACK_SIZE || len<=0) return 0;
  if(buffer==NULL) return 0;
  if(out_array==NULL || out_size<IMU_DECODED_AXIS)return 0;

  uint8_t pack_id = buffer[1];
  float temperature = static_cast<float>( static_cast<int16_t>(buffer[9]<<8 | buffer[8]) )/340.0f+36.25f;
  switch(pack_id){
  case PACK_ACCEL:
    out_array[0] = (short)(buffer[3]<<8 | buffer[2]) /MAX_RANGE*16;
    out_array[1] = (short)(buffer[5]<<8 | buffer[4]) /MAX_RANGE*16;
    out_array[2] = (short)(buffer[7]<<8 | buffer[6]) /MAX_RANGE*16;
    break;
  case PACK_GYRO:
    out_array[0] = static_cast<float>( static_cast<int16_t>(buffer[3]<<8 | buffer[2]) )*2000/MAX_RANGE;
    out_array[1] = static_cast<float>( static_cast<int16_t>(buffer[5]<<8 | buffer[4]) )*2000/MAX_RANGE;
    out_array[2] = static_cast<float>( static_cast<int16_t>(buffer[7]<<8 | buffer[6]) )*2000/MAX_RANGE;
    break;
  case PACK_EULAR:
    out_array[0] = static_cast<float>( static_cast<int16_t>(buffer[3]<<8 | buffer[2]) )*180/MAX_RANGE;
    out_array[1] = static_cast<float>( static_cast<int16_t>(buffer[5]<<8 | buffer[4]) )*180/MAX_RANGE;
    out_array[2] = static_cast<float>( static_cast<int16_t>(buffer[7]<<8 | buffer[6]) )*180/MAX_RANGE;
    break;
  }

  out_array[3] = temperature;
  return IMU_DECODED_AXIS;
}
