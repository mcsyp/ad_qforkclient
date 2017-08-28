#include "serialpix4flow.h"
#include <qdebug.h>
#include <QByteArray>

#include "mavlink_flow_v1.0/common/mavlink.h"

SerialPix4Flow::SerialPix4Flow(QObject *parent) : QSerialPort(parent)
{
  connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
}

bool SerialPix4Flow::begin(const QString port_path)
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
  setBaudRate(FLOW_BAUDRATE);
  last_flow_update_time_ = 0;

  return open(QIODevice::ReadWrite);
}

void SerialPix4Flow::onReadyRead()
{
  static mavlink_message_t msg;
  static mavlink_status_t  status;
  static mavlink_optical_flow_t flow_;
  static mavlink_optical_flow_rad_t flow_rad_;
  static int16_t last_temperature=0;

  QByteArray data = this->readAll();
  for(int i=0;i<data.size();++i){
    if(mavlink_parse_char(MAVLINK_COMM_0,data[i],&msg,&status)){
      uint64_t current_ms = QTime::currentTime().msecsSinceStartOfDay();
      /*rx a whole mavlink message*/
      switch(msg.msgid){
      case MAVLINK_MSG_ID_OPTICAL_FLOW:
        //step1.decode flow info
        mavlink_msg_optical_flow_decode(&msg,&flow_);

        //step2.compute time between frames
        if(last_flow_update_time_){
          time_between_frames_ = current_ms-last_flow_update_time_;
        }else{
          time_between_frames_ = 1;
        }

        //step3.output delta flow if quality passes
        if(flow_.quality>DEFAULT_THRESHOLD_QUALITY)
        {
          float dx = static_cast<float>(flow_.flow_x)/FLOW_XY_SCALE;
          float dy = static_cast<float>(flow_.flow_y)/FLOW_XY_SCALE;
          emit flowChanged(current_ms,dx,dy);
        }
        //last step
        last_flow_update_time_ =  current_ms;
        break;

      case MAVLINK_MSG_ID_OPTICAL_FLOW_RAD:
        mavlink_msg_optical_flow_rad_decode(&msg,&flow_rad_);
        if(abs(flow_rad_.temperature-last_temperature)>=TEMPERATURE_GAP){
          float t = static_cast<float>(flow_rad_.temperature)/FLOW_TEMP_SCALE;
          last_temperature =  flow_rad_.temperature;
          emit temperatureChanged(t);
        }
        break;
      }

    }
  }
}
