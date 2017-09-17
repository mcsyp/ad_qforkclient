#include "simserver.h"
#include <QDebug>
#include <QDateTime>
#include "servercmdid.h"
using namespace server_protocol_cmd;
SimServer::SimServer(QObject* parent):QTcpServer(parent)
{
  connect(this,SIGNAL(newConnection()),this, SLOT(onNewConnection()));
  connect(&protocol_,SIGNAL(payloadReady(int,QByteArray)),this,SLOT(onPayloadReady(int,QByteArray)));
}
void SimServer::StartService(int port){
  this->listen(QHostAddress::Any,port);
  qDebug()<<tr("[%1,%2]fork simulator service started on %3:%4")
            .arg(__FILE__).arg(__LINE__)
            .arg(this->serverAddress().toString())
            .arg(port);
}

void SimServer::onNewConnection(){
  QTcpSocket * socket = this->nextPendingConnection();
  connect(socket,SIGNAL(readyRead()),this,SLOT(onClientReadyRead()));
  connect(socket,SIGNAL(disconnected()),this,SLOT(onClientDisconnected()));

  printf("[%s,%d] new incomming socket\n",__FUNCTION__,__LINE__);
  ptr_soket_=socket;
}
void SimServer::onClientDisconnected(){
  printf("[%s,%d] socket disconnected\n",__FUNCTION__,__LINE__);
}
void SimServer::onClientReadyRead(){
  QByteArray rx_array = ptr_soket_->readAll();
  protocol_.PushToProtocol(rx_array);
#if 0
  qDebug()<<tr("[%1,%2] data_len=%3").arg(__FILE__).arg(__LINE__).arg(rx_array.length());
  for(int i=0;i<rx_array.length();++i){
    printf("0x%x ",(uint8_t)rx_array[i]);
  }
  printf("\n");
#endif
}

void SimServer::onPayloadReady(int cmdid, QByteArray payload){
  qDebug()<<tr("[%1,%2] cmdid=%3,payload_len=%4").arg(__FILE__).arg(__LINE__).arg(cmdid).arg(payload.length());
  switch(cmdid){
    case SERVER_LOGIN_REQ:{
        char devid[32];
        memcpy(devid,payload.data(),payload.size());
        devid[payload.size()] = '\0';
        qDebug()<<tr("Device ID:%1").arg(devid);
      }
      break;
    case SERVER_TIME_REQ:
    case SERVER_HB_REQ:{
        uint32_t  time = static_cast<uint32_t>(QDateTime::currentSecsSinceEpoch());
        SendBytes(SERVER_TIME_ACK,(uint8_t*)&time,sizeof(time));
        qDebug()<<tr("Replied time request:%1").arg(time);
      }
      break;
    case SERVER_MASTER_BATTERY_REQ:{
        float master_volt;
        memcpy((uint8_t*)&master_volt,(uint8_t*)payload.data(),sizeof(float));
        qDebug()<<tr("Get master volt uploaded:%1").arg(master_volt);
      }
      break;
    case SERVER_SLAVE_BATTERY_REQ:{
        float slave_volt;
        memcpy((uint8_t*)&slave_volt,(uint8_t*)payload.data(),sizeof(float));
        qDebug()<<tr("Get slave volt uploaded:%1").arg(slave_volt);
      }
      break;
    case SERVER_UPLOAD_REQ:{
        qDebug()<<tr("File:%1").arg(QString(payload));
        qDebug()<<tr("Replied file ack");
        SendBytes(SERVER_UPLOAD_ACK,NULL,0);
      }
      break;
    case SERVER_LOG_REQ:{
        qDebug()<<tr("Log:%1").arg(QString(payload));
      }
      break;
  }
}
void SimServer::SendBytes(uint16_t cmd_id, uint8_t * data, uint16_t len){
  const int head_size = 32;
  uint8_t head_buffer[head_size];

  //step1. fill a header
  int head_len = ServerProtocol::FillHead(cmd_id,len,head_buffer,head_size);

  //step2. send out the header and the data
  if(ptr_soket_){
    ptr_soket_->write((char*)head_buffer,head_len);
    if(data && len){
      ptr_soket_->write((char*)data,len);
    }
  }
}
