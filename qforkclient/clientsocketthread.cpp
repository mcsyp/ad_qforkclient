#include "clientsocketthread.h"
#include "servercmdid.h"
#include <QTime>
#include <QFile>

using namespace server_protocol_cmd;

ClientSocketThread::ClientSocketThread(QObject *parent) : QTcpSocket(parent)
{
   connect(this,SIGNAL(connected()),this,SLOT(onConnected()));
   connect(this,SIGNAL(disconnected()),this,SLOT(onDisconnected()));
   connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
   connect(&protocol_,SIGNAL(payloadReady(int,QByteArray)),this,SLOT(onPayloadReady(int,QByteArray)));

   moveToThread(&event_thread_);

   this->server_time_ = QTime::currentTime().msecsSinceStartOfDay()/1000;
   this->local_time_offset_ = this->server_time_;
   this->volt_master_ = 0.0f;
   this->volt_slave_ = 0.0f;
}

bool ClientSocketThread::Begin(ConfigParser::ConfigMap &configs)
{
  //step1.server address
  if(configs.contains(CONFIG_KEY_SERVER_ADDRESS)){
    this->server_address_ = configs[CONFIG_KEY_SERVER_ADDRESS];
  }else{
    return false;
  }

  this->devid_ = "unknown";
  if(configs.contains(CONFIG_KEY_DEVID)){
    devid_ = configs[CONFIG_KEY_DEVID];
  }

  if(configs.contains(CONFIG_KEY_SERVER_PORT)){
    this->server_port_ = configs[CONFIG_KEY_SERVER_PORT].toInt();
  }else{
    return false;
  }
  qDebug()<<tr("[%1,%2]server address:%3:%4")
            .arg(__FILE__).arg(__LINE__)
            .arg(this->server_address_)
            .arg(this->server_port_);

  //step2. heartbeat update
  hb_timeout_ = SERVER_HB_TIMEOUT;
  if(configs.contains(CONFIG_KEY_SERVER_HB_TIMEOUT)){
    this->hb_timeout_ = configs[CONFIG_KEY_SERVER_HB_TIMEOUT].toInt();
  }
  hb_ts_ = QTime::currentTime().msecsSinceStartOfDay();
  qDebug()<<tr("[%1,%2]server heartbeat udpate timeout:%3").arg(__FILE__).arg(__LINE__).arg(hb_timeout_);

  //event thread
  event_thread_.start();
  return true;
}

void ClientSocketThread::onConnected()
{
  qDebug()<<tr("[%1,%2]server connected").arg(__FILE__).arg(__LINE__);

  //step2.login message
  const int head_buf_size=32;
  uint8_t head[head_buf_size];
  int message_len = ServerProtocol::FillHead(SERVER_LOGIN_REQ,devid_.length(),head,head_buf_size);
  write((char*)head,message_len);
  write((char*)devid_.toLatin1().data(),devid_.length());

  //step3.time request
  QThread::msleep(50);
  memset(head,0,head_buf_size);
  message_len = ServerProtocol::FillHead(SERVER_TIME_REQ,0,head,head_buf_size);
  write((char*)head,message_len);
}

void ClientSocketThread::onDisconnected()
{
  qDebug()<<tr("[%1,%2]server disconnected").arg(__FILE__).arg(__LINE__);
}

void ClientSocketThread::onReadyRead(){
  QByteArray data = this->readAll();
  this->protocol_.PushToProtocol(data);
  //qDebug()<<tr("[%1,%2] rx %3 bytes").arg(__FILE__).arg(__LINE__).arg(data.size());
}

void ClientSocketThread::onPayloadReady(int cmdid, QByteArray payload)
{
  switch(cmdid){
    case SERVER_TIME_ACK:
    case SERVER_HB_ACK:{
        uint32_t t;
        memcpy((uint8_t*)&t,(uint8_t*)payload.data(),payload.size());
        server_time_ = t;
        local_time_offset_ = QTime::currentTime().msecsSinceStartOfDay()/1000;
        qDebug()<<tr("[%1,%2]server_time:%3,local_time:%4")
                  .arg(__FILE__).arg(__LINE__)
                  .arg(server_time_).arg(local_time_offset_);
      }
      break;
   case SERVER_UPLOAD_ACK:{
        //TODO: remove the upload.csv file
        qDebug()<<tr("[%1,%2]upload file ack.remove file:%3")
                  .arg(__FILE__).arg(__LINE__).arg(this->upload_path_);
        QFile::remove(this->upload_path_);
      }
      break;
  }
}

void ClientSocketThread::onReconnectTimeout()
{
  //step1.if server is not connected
  if(this->state()!=QTcpSocket::ConnectedState){
    connectToHost(this->server_address_,this->server_port_);
    return;
  }

  //step2.check if hb task is triggered
  int ts = QTime::currentTime().msecsSinceStartOfDay();
  if(ts-hb_ts_>hb_timeout_){
    hb_ts_ = ts;

    const int head_size = 32;
    uint8_t head[head_size];
    //step1.update hb
    do{
      int head_len = ServerProtocol::FillHead(SERVER_HB_REQ,0,head,head_size);
      write((char*)head,head_len);
      //qDebug()<<tr("[%1,%2]HB updated").arg(__FILE__).arg(__LINE__);
    }while(0);


    //step3. i2c_reader
    I2CReader ads_reader_;
    if(ads_reader_.Begin(ADS1000_DEV) && ads_reader_.SetSlave(ADS1000_ADDR)){
      const int rd_size=3;
      uint8_t rd_buffer[rd_size];
      uint16_t volt_raw;

      int len = ads_reader_.ReadBytes(rd_buffer,rd_size);
      if(len<0)return;

      //step1.convert raw.
      uint8_t* ptr_data = (uint8_t*)&volt_raw;
      ptr_data[1] = rd_buffer[0];
      ptr_data[0] = rd_buffer[1];

      //step2.convert to voltage:
      float p_v = static_cast<float>(volt_raw)/2048.0f;
      volt_master_ = p_v*6.6f;
      printf("[%s,%d]p:%.2f, voltage:%.2f\n",__FILE__,__LINE__,p_v,volt_master_);
    }

    //step2.update master volt
    if(volt_master_){
      int head_len = ServerProtocol::FillHead(SERVER_MASTER_BATTERY_REQ,sizeof(this->volt_master_),head,head_size);
      write((char*)head,head_len);
      write((char*)(&volt_master_),sizeof(volt_master_));
    }

    //step3.update slave volt
    if(volt_slave_){
      int head_len = ServerProtocol::FillHead(SERVER_SLAVE_BATTERY_REQ,sizeof(this->volt_slave_),head,head_size);
      write((char*)head,head_len);
      write((char*)(&volt_slave_),sizeof(volt_slave_));
    }
  }
}

void ClientSocketThread::onUploadTimeout(QString file_path)
{
  const int head_size=32;
  uint8_t head_buffer[head_size];
  int data_size=0;

  if(!this->isOpen()){
    qDebug()<<tr("[%1,%2]device is not open").arg(__FILE__).arg(__LINE__);
    return;
  }
  this->upload_path_ = file_path;

  QFile file(this->upload_path_);
  if(!file.open(QIODevice::ReadOnly)){
    qDebug()<<tr("[%1,%2]fail to open %3").arg(__FILE__).arg(__LINE__).arg(upload_path_);
    return;
  }

  int head_len = ServerProtocol::FillHead(SERVER_UPLOAD_REQ,file.size(),head_buffer,head_size);
  write((char*)head_buffer,head_len);

  QTextStream stream(&file);
  while(!stream.atEnd()){
    QString data = stream.read(256);
    write(data.toLatin1().data(),data.length());
    data_size+=data.size();
  }
  file.close();
  qDebug()<<tr("[%1,%2]uploaded file[%3/%4] done").arg(__FILE__).arg(__LINE__).arg(data_size).arg(file.size());
}

