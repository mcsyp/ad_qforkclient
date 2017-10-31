#include "imuserialthread.h"
#include <QList>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QTime>
#include "qdebug.h"

using namespace qri_neuron_lib;
ImuSerialThread::ImuSerialThread(QObject *parent) : QSerialPort(parent){
  connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
  connect(this,SIGNAL(postBegin(QString)),this,SLOT(onBegin(QString)));
  connect(&decoder_,SIGNAL(readyRead()),this,SLOT(onImuReadyRead()));

  this->moveToThread(&worker_thread_);
  worker_thread_.start();

  ptr_gyro_frame_ = new DataFrame(GYRO_MSE_SIZE,GYRO_MSE_COLS);
}

bool ImuSerialThread::begin(QString portname)
{
  bool ret=false;
  int index=0;

  //list all ports
  QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
  if(list.size()==0){
    qDebug()<<tr("[%1,%2]No available serial port in system.").arg(__FILE__).arg(__LINE__);
    return false;
  }

  //search for target ports
  foreach(QSerialPortInfo info, list){
    qDebug()<<tr("serial port[%1]:%2").arg(index++).arg(info.portName());
    if(info.portName()==portname){
      ret = true;
      break;
    }
  }

  if(!ret){
    qDebug()<<tr("[%1,%2]Fail to find the %3").arg(__FILE__).arg(__LINE__).arg(portname);
    return false;
  }

  //if everything success
  emit postBegin(portname);
  return true;
}

bool ImuSerialThread::searchRecord(int ts, ImuSerialThread::ImuRecordList &list, ImuSerialThread::imu_record_t *out_record)
{
  int min_dist=MSEC_ONE_DAY;
  imu_record_t * ptr_record=NULL;

  record_lock_.tryLock(RECORD_LOCK_TIMEOUT);
  foreach (imu_record_t iter, list) {
    int d = abs(iter.ts-ts);
    if(d<min_dist){
      min_dist = d;
      ptr_record = &iter;
    }
  }
  record_lock_.unlock();

  if(min_dist<=RECORD_TIMEGAP){
    memcpy((uint8_t*)out_record,(uint8_t*)ptr_record,sizeof(imu_record_t));
    return true;
  }
  return false;
}

void ImuSerialThread::onReadyRead(){
  QByteArray rx_data = this->readAll();
  for(int i=0;i<rx_data.length();++i){
    decoder_.push((uint8_t)rx_data.at(i));
  }
}

void ImuSerialThread::onImuReadyRead(){
  int ts = QTime::currentTime().msecsSinceStartOfDay();
  //TODO: process your imu data
  switch(decoder_.readID()){
    case ImuDecoder::PACK_GYRO:{
        //step1. push data to frame
        float imu_data[ImuDecoder::IMU_DECODED_AXIS];
        int imu_len = decoder_.readData(imu_data,ImuDecoder::IMU_DECODED_AXIS);
        if(!imu_len)return;

        this->ptr_gyro_frame_->push(imu_data,GYRO_MSE_COLS);

        //step2. compute mse and save to record list
        if(ptr_gyro_frame_->full()){
          //step0.init record
          imu_record_t record;
          record.ts = ts;

          //step1.compute mse
          for(int i=0;i<GYRO_MSE_COLS;++i){
            record.axis[i] = feature_mse_.process(ptr_gyro_frame_->readColumnData(i),ptr_gyro_frame_->rowLength());
          }

          //step2. save to record list
          mse_list_.push_back(record);
          emit imuRecordUpdate(ts);

          //step3.pop the window
          ptr_gyro_frame_->pop(GYRO_MSE_DELTA);
        }

        //step3. check mse record list size
        if(mse_list_.size()>MSE_RECORD_SIZE){
          record_lock_.tryLock(RECORD_LOCK_TIMEOUT);
          mse_list_.clear();
          record_lock_.unlock();
        }
      }
      break;
    case ImuDecoder::PACK_EULAR:{
        //step1. init record structure
        imu_record_t record;
        int imu_len = decoder_.readData(record.axis,ImuDecoder::IMU_DECODED_AXIS);
        record.ts = ts;

        //step2. save record
        if(imu_len){
          //if available imu result
          eular_list_.push_back(record);
          emit imuRecordUpdate(ts);
        }

        //step3.check eular list size
        if(eular_list_.size()>EULAR_RECORD_SIZE){
          record_lock_.tryLock(RECORD_LOCK_TIMEOUT);
          eular_list_.clear();
          record_lock_.unlock();
        }
      }
      break;
  }
}

void ImuSerialThread::onBegin(QString portname)
{
  setPortName(portname);
  setBaudRate(MY_BAUDRATE);
  if(!open(QIODevice::ReadWrite)){
    qDebug()<<tr("[%1,%2]fail to open serial device");
  }
}
