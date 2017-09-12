#include "forkliftclient.h"
#include <qdebug.h>
#include <QTime>
#include <QDateTime>
#include <QTime>
#include <QFile>
#include "serverprotocol.h"
#include "servercmdid.h"
using namespace server_protocol_cmd;
ForkliftClient::ForkliftClient(QObject *parent) : QTcpSocket(parent)
{
  connect(this,SIGNAL(connected()),this,SLOT(onConnected()));
  connect(this,SIGNAL(disconnected()),this,SLOT(onDisconnected()));
  connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));

  connect(&reconnect_timer_,SIGNAL(timeout()),this,SLOT(onReconnectTimeout()));
  connect(&upload_timer_,SIGNAL(timeout()),this,SLOT(onUploadTimeout()));
  connect(&hb_timer_,SIGNAL(timeout()),this,SLOT(onHBTimeout()));
  connect(&record_timer_,SIGNAL(timeout()),this,SLOT(onRecordTimeout()));

  connect(&protocol_,SIGNAL(payloadReady(int,QByteArray)),this,SLOT(onPayloadReady(int,QByteArray)));
  connect(&slave_ble_,SIGNAL(bleRxDataReady()),this,SLOT(onSlaveBleReady()));
  connect(&slave_ble_,SIGNAL(bleDisconnected()),this,SLOT(onSlaveBleDisconnected()));

  //update distance worker
  connect(&dist_estimater_,SIGNAL(trajectoryUpdated()),this,SLOT(onTrajectoryUpdated()));

  //server time init
  server_time_ = QDateTime::currentMSecsSinceEpoch()/1000;
  local_time_offset_ = QTime::currentTime().msecsSinceStartOfDay()/1000;
}
ForkliftClient::~ForkliftClient(){
}

bool ForkliftClient::Begin(ConfigParser::ConfigMap &configs){
  do{//load server settings
    server_address_ = "";
    server_port_=0;
    server_retry_timeout_=SERVER_RECONNECTION_TIMEOUT;
    if(configs.contains(CONFIG_KEY_SERVER_ADDRESS)){
      server_address_ = configs[CONFIG_KEY_SERVER_ADDRESS];
    }
    if(configs.contains(CONFIG_KEY_SERVER_PORT)){
      server_port_ = configs[CONFIG_KEY_SERVER_PORT].toInt();
    }
    if(configs.contains(CONFIG_KEY_SERVER_RECONNECT_TIMEOUT)){
      server_retry_timeout_ = configs[CONFIG_KEY_SERVER_RECONNECT_TIMEOUT].toInt();
    }
    if(server_address_.isEmpty() || server_port_==0){
      qDebug()<<tr("[%1,%2]invalid server parameters. quit").arg(__FILE__).arg(__LINE__);
      return false;
    }

    qDebug()<<tr("[%1,%2]connecting to server:%3:%4 at interval %5 ms")
              .arg(__FILE__).arg(__LINE__)
              .arg(server_address_)
              .arg(server_port_)
              .arg(server_retry_timeout_);
    //start the reconnection
    connectToHost(server_address_,server_port_);
  }while(0);

  do{//local name and slave_ble
    devid_ = "unknown";
    if(configs.contains(CONFIG_KEY_DEVID)){
      devid_ = configs[CONFIG_KEY_DEVID];
    }

    this->slave_mac_ = "";
    if(configs.contains(CONFIG_KEY_SLAVE_BLE)){
      this->slave_mac_ = configs[CONFIG_KEY_SLAVE_BLE];
    }
    QString portname = configs[CONFGI_KEY_BLE_SERIAL];
    if(!slave_ble_.Begin(portname)){
      qDebug()<<tr("[%1,%2]fail to open ble port:%3").arg(__FILE__).arg(__LINE__).arg(portname);
      return false;
    }
    //slave_ble_.ResetBle();
    //QThread::msleep(500);
    //slave_ble_.HostBle(true);
    //QThread::msleep(100);
    slave_ble_.ConnectToBle(slave_mac_);
    memset((char*)&ble_info_,0,sizeof(ble_info_));

    qDebug()<<tr("[%1,%2]device id:%3, connecting to ble:%4").arg(__FILE__).arg(__LINE__).arg(devid_).arg(slave_mac_);
  }while(0);

  do{//local settings
    upload_timeout_ = SERVER_UPLOAD_TIMEOUT;
    if(configs.contains(CONFIG_KEY_SERVER_UPLOAD_TIMEOUT)){
      upload_timeout_ = configs[CONFIG_KEY_SERVER_UPLOAD_TIMEOUT].toInt();
    }
    hb_timeout_ = SERVER_HB_TIMEOUT;
    if(configs.contains(CONFIG_KEY_SERVER_HB_TIMEOUT)){
      hb_timeout_ = configs[CONFIG_KEY_SERVER_HB_TIMEOUT].toInt();
    }
    qDebug()<<tr("[%1,%2]upload timeout:%3s, heartbeat timeout:%4")
              .arg(__FILE__).arg(__LINE__)
              .arg(upload_timeout_)
              .arg(hb_timeout_);
  }while(0);

  do{//open record file
    record_timeout_ = RECORD_TIMEOUT;
    if(configs.contains(CONFIG_KEY_RECORD_TIMEOUT)){
      record_timeout_ = configs[CONFIG_KEY_RECORD_TIMEOUT].toInt();
    }

    QString record_tmp_path_="./record_tmp.csv";
    if(configs.contains(CONFIG_KEY_RECORD_TEMP)){
      record_tmp_path_ = configs[CONFIG_KEY_RECORD_TEMP];
    }

    record_tmp_file_.setFileName(record_tmp_path_);
    if(!record_tmp_file_.open(QIODevice::ReadWrite)){
      qDebug()<<tr("[%1,%2]Fail to open the record tmp:%3").arg(__FILE__).arg(__LINE__).arg(record_tmp_path_);
      return false;
    }

    record_tmp_text_.setDevice(&record_tmp_file_);
    qDebug()<<tr("[%1,%2]record temp:%3 at interval:%4")
              .arg(__FILE__).arg(__LINE__)
              .arg(record_tmp_path_)
              .arg(record_timeout_);

    //set the file operate position
    record_tmp_text_.seek(record_tmp_file_.size());
    record_tmp_text_<<"hello"<<endl;
  }while(0);

  do{//load estimater
    if(!dist_estimater_.Begin(configs)){
      qDebug()<<tr("[%1,%2]Fail to begin the esimator").arg(__FILE__).arg(__LINE__);
      return false;
    }
    qDebug()<<tr("[%1,%2]distance estimater initialized").arg(__FILE__).arg(__LINE__);
  }while(0);

  //start server reconnection timer
  reconnect_timer_.start(server_retry_timeout_);

  return true;
}

void ForkliftClient::UploadFile(QString path)
{
  const int head_size=32;
  uint8_t head_buffer[head_size];
  int data_size=0;

  QFile file(path);
  if(!file.open(QIODevice::ReadOnly)){
    qDebug()<<tr("[%1,%2]fail to open %3").arg(__FILE__).arg(__LINE__).arg(path);
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
  qDebug()<<tr("[%1,%2]uploaded file[%3/%4]").arg(__FILE__).arg(__LINE__).arg(data_size).arg(file.size());
}
void ForkliftClient::onReconnectTimeout(){
#if 0
  qDebug()<<tr("[%1,%2]connecting to server:%3:%4 at interval %5 ms")
            .arg(__FILE__).arg(__LINE__)
            .arg(server_address_)
            .arg(server_port_)
            .arg(server_retry_timeout_);
#endif
  //start the reconnection
  connectToHost(server_address_,server_port_);
}


void ForkliftClient::onTrajectoryUpdated()
{
  dist_track_.timestamp = dist_estimater_.LastUpdateTimestamp();
  dist_track_.heading = dist_estimater_.Heading();
  dist_track_.velocity = dist_estimater_.Velocity();
  dist_track_.distance = dist_estimater_.Distance();
  dist_track_.distance_abs = dist_estimater_.DistanceAbs();

  dist_estimater_.DistanceDelta(&(dist_track_.delta_x),&(dist_track_.delta_y));
  dist_estimater_.Distance(&(dist_track_.distance_x),&(dist_track_.distance_y));
#if 0
  qDebug()<<tr("[%1,%2]ts:%3, heading:%4, dist:%5")
            .arg(__FILE__).arg(__LINE__)
            .arg(dist_track_.timestamp)
            .arg(dist_track_.heading)
            .arg(dist_track_.distance);
#endif
}
void ForkliftClient::onHBTimeout()
{
  uint8_t head[32];
  int message_size = ServerProtocol::FillHead(SERVER_HB_REQ,0,head,32);
  write((char*)head,message_size);
}

void ForkliftClient::onRecordTimeout()
{
  int local_time = QTime::currentTime().msecsSinceStartOfDay()/1000;
  int ts = server_time_ +(local_time-local_time_offset_);
  int moving_flag = 0;
  if(dist_track_.velocity){
    moving_flag=1;
  }

#if 0
  len = rt_sprintf(str_reocrd,
                    "%lu,%s,%d,%d,%u,%d,%d,%d\r\n",
                    timestamp,
                    str_devid_,
                    eular_yaw_,
                    grid_counter_,
                    total_grid_,
                    gyro_moving_flag,
                    slave_info_.ir_dist,
                    slave_info_.laser_dist);
#endif
  record_tmp_text_<<ts<<",";
  record_tmp_text_<<devid_<<",";
  record_tmp_text_<<dist_track_.heading<<",";
  record_tmp_text_<<dist_track_.distance<<",";
  record_tmp_text_<<dist_track_.distance_abs<<",";
  record_tmp_text_<<moving_flag<<",";
  record_tmp_text_<<ble_info_.ir_dist<<",";
  record_tmp_text_<<ble_info_.laser_dist;
  record_tmp_text_<<endl;
}
void ForkliftClient::onUploadTimeout()
{
  //TODO:combine tmp to upload and transmit upload file
  record_tmp_text_.flush();
  if(!record_tmp_file_.copy(RECORD_UPLOAD_PATH)){
    //false means the file exists
    QFile file(RECORD_UPLOAD_PATH);
    if(!file.open(QIODevice::ReadWrite)){
      QFile::remove(RECORD_UPLOAD_PATH);
      qDebug()<<tr("[%1,%2]Fail to open the record file:%3").arg(__FILE__).arg(__LINE__).arg(RECORD_UPLOAD_PATH);
      return;
    }
    //cat the tmp file to upload file
    QTextStream stream(&file);
    stream.seek(file.size());
    record_tmp_text_.seek(0);
    while(!record_tmp_text_.atEnd()){
      stream<<record_tmp_text_.read(1024);
    }
    file.close();
  }
  record_tmp_file_.close();
  record_tmp_file_.remove();
  if(!record_tmp_file_.open(QIODevice::ReadWrite)){
    qDebug()<<tr("[%1,%2]Fail to open the record file.").arg(__FILE__).arg(__LINE__);
    return;
  }
  record_tmp_text_.setDevice(&record_tmp_file_);
  record_tmp_text_.seek(0);

  UploadFile(RECORD_UPLOAD_PATH);
}
void ForkliftClient::onPayloadReady(int cmdid, QByteArray payload)
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
        qDebug()<<tr("[%1,%2]upload file ack")
                  .arg(__FILE__).arg(__LINE__);
        QFile::remove(RECORD_UPLOAD_PATH);
      }
      break;
  }

}

void ForkliftClient::onSlaveBleReady()
{
  ble_info_ = slave_ble_.SlaveInfo();
  qDebug()<<tr("[%1,%2]ir_dist:%3, laser_dist:%4, volt:%5")
            .arg(__FILE__).arg(__LINE__)
            .arg(ble_info_.ir_dist)
            .arg(ble_info_.laser_dist)
            .arg((float)ble_info_.volt/1000.0f);
}

void ForkliftClient::onSlaveBleDisconnected()
{
  qDebug()<<tr("[%1,%2]ble slave disconnected connecting:%3").arg(__FILE__).arg(__LINE__).arg(slave_mac_);
  slave_ble_.ConnectToBle(slave_mac_);
}

void ForkliftClient::onConnected()
{
  qDebug()<<tr("[%1,%2]server connected").arg(__FILE__).arg(__LINE__);
  //step1.timer settings
  reconnect_timer_.stop();
  hb_timer_.start(hb_timeout_);
  record_timer_.start(record_timeout_);
  upload_timer_.start(upload_timeout_);

  //step2.login message
  const int head_buf_size=32;
  uint8_t head[head_buf_size];
  int message_len = ServerProtocol::FillHead(SERVER_LOGIN_REQ,devid_.length(),head,head_buf_size);
  write((char*)head,message_len);
  write((char*)devid_.toLatin1().data(),devid_.length());

  //step3.time request
  QThread::msleep(100);
  memset(head,0,head_buf_size);
  message_len = ServerProtocol::FillHead(SERVER_TIME_REQ,0,head,head_buf_size);
  write((char*)head,message_len);
}

void ForkliftClient::onDisconnected()
{
  qDebug()<<tr("[%1,%2]server disconnected").arg(__FILE__).arg(__LINE__);
  reconnect_timer_.start(server_retry_timeout_);
  hb_timer_.stop();
  record_timer_.stop();
  upload_timer_.stop();
}

void ForkliftClient::onReadyRead()
{
  QByteArray data = readAll();
  protocol_.PushToProtocol(data);
  //qDebug()<<tr("[%1,%2]rx message:%3").arg(__FILE__).arg(__LINE__).arg(QString(data));
}
