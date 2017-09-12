#include "serialble.h"
#include <qdebug.h>
#include <QByteArray>
#include <QThread>
#include <math.h>
SerialBle::SerialBle(QObject *parent) : QSerialPort(parent)
{
  connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
  connect(&hdlc_,SIGNAL(hdlcFrameDecoded(char*,int)),this,SLOT(onHdlcFrameDecoded(char*,int)));
  connect(&rx_hb_timer_,SIGNAL(timeout()),this,SLOT(onHBTimeout()));
  last_rx_time_ = 0;
}

bool SerialBle::Begin(const QString port_path)
{
  //list all ports
  QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
  if(list.size()==0){
    qDebug()<<tr("[%1,%2]No available serial port in system.").arg(__FILE__).arg(__LINE__);
    return false;
  }

  //search for target ports
  bool ret=false;
  foreach(QSerialPortInfo info, list){
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
  rx_hb_timer_.start(STATE_CHECK_TIMEOUT);
  if(open(QIODevice::ReadWrite)){
    return true;
  }
  return false;
}

void SerialBle::HostBle(bool b)
{
  if(b){//host mode
    const char * str = "AT+ROLE1\r\n";
    writeData(str,strlen(str));
  }else{
    const char * str = "AT+ROLE0\r\n";
    writeData(str,strlen(str));
  }
}

void SerialBle::ConnectToBle(QString mac)
{
  char cmd[64];
  //printf("[%s,%d]connecting to %s\n",__FILE__,__LINE__,mac.toLatin1().data());
  sprintf(cmd,"AT+CONA%s\r\n",mac.toLatin1().data());
  writeData(cmd,strlen(cmd));
}

void SerialBle::ResetBle(){
  const char * str = "AT+RESET\r\n";
  writeData(str,strlen(str));
}

void SerialBle::onReadyRead()
{
  QByteArray data = this->readAll();
  for(int i=0;i<data.length();++i){
    hdlc_.Push((uint8_t)data[i]);
  }
}


void SerialBle::onHdlcFrameDecoded(char* frame, int size)
{
  memcpy((uint8_t*)&slave_info_,frame,size);
  last_rx_time_ = QTime::currentTime().msecsSinceStartOfDay();
  emit bleRxDataReady();
}

void SerialBle::onHBTimeout()
{
  qint64 t = QTime::currentTime().msecsSinceStartOfDay();
  if(abs(t-last_rx_time_)>LAST_RX_TIMEOUT){
    //if no rx message
    last_rx_time_ = QTime::currentTime().msecsSinceStartOfDay();
    emit bleDisconnected();
  }
}
