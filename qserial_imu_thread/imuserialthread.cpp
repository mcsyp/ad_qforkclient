#include "imuserialthread.h"
#include <QList>
#include <QSerialPortInfo>
#include <QByteArray>
#include "qdebug.h"

ImuSerialThread::ImuSerialThread(QObject *parent) : QSerialPort(parent){
  connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
  connect(this,SIGNAL(postBegin(QString)),this,SLOT(onBegin(QString)));
  connect(&decoder_,SIGNAL(readyRead()),this,SLOT(onImuReadyRead()));

  this->moveToThread(&worker_thread_);
  worker_thread_.start();
}

bool ImuSerialThread::begin(const char *portname)
{
  bool ret=false;
  int index=0;

  if(portname==NULL)return false;
  QString port_path = QString(portname);

  //list all ports
  QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
  if(list.size()==0){
    qDebug()<<tr("[%1,%2]No available serial port in system.").arg(__FILE__).arg(__LINE__);
    return false;
  }

  //search for target ports
  foreach(QSerialPortInfo info, list){
    qDebug()<<tr("serial port[%1]:%2").arg(index++).arg(info.portName());
    if(info.portName()==port_path){
      ret = true;
      break;
    }
  }

  if(!ret){
    qDebug()<<tr("[%1,%2]Fail to find the %3").arg(__FILE__).arg(__LINE__).arg(port_path);
    return false;
  }

  emit postBegin(port_path);
  return true;
}

void ImuSerialThread::onReadyRead(){
  QByteArray rx_data = this->readAll();
  for(int i=0;i<rx_data.length();++i){
    decoder_.push((uint8_t)rx_data.at(i));
  }
}

void ImuSerialThread::onImuReadyRead(){
  const int imu_size = ImuDecoder::IMU_DECODED_AXIS;
  float imu_data[imu_size];
  int imu_len=0;
  imu_len = decoder_.readData(imu_data,imu_size);

  if(decoder_.readID()==ImuDecoder::PACK_EULAR){
    printf("[%s,%d]imu_data[0x%x]:",__FILE__,__LINE__,decoder_.readID());
    for(int i=0;i<imu_len;++i){
      printf("%3.2f  ",imu_data[i]);
    }
    printf("\n");
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
