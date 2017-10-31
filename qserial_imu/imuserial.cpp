#include "imuserial.h"
#include <QList>
#include <QSerialPortInfo>
#include <QByteArray>
#include "qdebug.h"

ImuSerial::ImuSerial(QObject *parent) : QSerialPort(parent){
  connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
  connect(&decoder_,SIGNAL(readyRead()),this,SLOT(onImuReadyRead()));
}

bool ImuSerial::begin(const char *portname)
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
      setPort(info);
      ret = true;
      break;
    }
  }
  if(!ret){
    qDebug()<<tr("[%1,%2]Fail to find the %3").arg(__FILE__).arg(__LINE__).arg(port_path);
    return false;
  }

  //set baudrate
  setBaudRate(MY_BUADRATE);
  if(open(QIODevice::ReadWrite)){
    return true;
  }
  qDebug()<<tr("[%1,%2]Fail to open the %3").arg(__FILE__).arg(__LINE__).arg(port_path);
  return false;
}

void ImuSerial::onReadyRead(){
  QByteArray rx_data = this->readAll();
  for(int i=0;i<rx_data.length();++i){
    decoder_.push((uint8_t)rx_data.at(i));
  }
}

void ImuSerial::onImuReadyRead(){
  const int imu_size = ImuDecoder::IMU_DECODED_AXIS;
  float imu_data[imu_size];
  int imu_len=0;
  imu_len = decoder_.readData(imu_data,imu_size);

  if(decoder_.readID()==ImuDecoder::PACK_EULAR){
    printf("[%s,%d]imu_data[0x%x]:",__FILE__,__LINE__,decoder_.readID());
    for(int i=0;i<imu_len;++i){
      printf("%.2f,",imu_data[i]);
    }
    printf("\n");
  }
}
