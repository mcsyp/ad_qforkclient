#include "seriallaser.h"
#include <qdebug.h>
#include <QByteArray>
#include <math.h>
SerialLaser::SerialLaser(QObject *parent) : QSerialPort(parent)
{
  setThreshold(DEFAULT_MIN_DIST_THRESHOLD,DEFAULT_MAX_DIST_THRESHOLD);
  connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
}

bool SerialLaser::begin(const QString port_path)
{
  //list all ports
  QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
  if(list.size()==0){
    qDebug()<<tr("[%1,%2]No available serial port in system.").arg(__FILE__).arg(__LINE__);
    return false;
  }

  //search for target ports
  bool ret=false;
  qDebug()<<tr("Searching for:%1").arg(port_path);
  foreach(QSerialPortInfo info, list){
    qDebug()<<tr("serialport:[%1]").arg(info.portName());
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
  setBaudRate(LASER_BAUDRATE);

  if(open(QIODevice::ReadWrite)){
    this->writeData("O",1);
    return true;
  }
  return false;
}
void SerialLaser::setThreshold(float min, float max){
  if(min>=max)return;
  this->min_gap_ = min;
  this->max_gap_ = max;
}

bool SerialLaser::checkValid(float current){
  static float last_dist=0.0f;
  if(last_dist==0.0f){
    last_dist = current;
    return true;
  }
  float gap = fabs(last_dist-current);
  if(gap>this->max_gap_ || gap<this->min_gap_){
    return false;
  }
  last_dist = current;
  return true;
}

float SerialLaser::parseResponseMessage(char *str_rx)
{
  char* offset_start = strchr(str_rx,':');
  if(offset_start==NULL)return 0.0f;

  char* offset_end = strchr(str_rx, 'm');
  if(offset_end==NULL)return 0.0f;

  int str_start = offset_start-str_rx+1;
  offset_end[0]='\0';
  float val = atof(str_rx+str_start);

  return val;
}

void SerialLaser::trigger(){
  this->writeData("D",1);
}

void SerialLaser::onReadyRead()
{
  char* str_rx = this->readAll().data();
  float dist = parseResponseMessage(str_rx);
  if(dist && checkValid(dist)){
    uint64_t current_ms = QTime::currentTime().msecsSinceStartOfDay();
    emit distanceChanged(current_ms,dist);//in meter
  }
}