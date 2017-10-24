#include "forkliftclient.h"
#include <qdebug.h>
#include <QTime>
#include <QDateTime>
#include <QTime>
#include <QFile>
#include "neogpio.h"

#include "serverprotocol.h"
#include "servercmdid.h"
using namespace server_protocol_cmd;
ForkliftClient::ForkliftClient(QObject *parent) : QObject(parent)
{
  connect(&reconnect_timer_,SIGNAL(timeout()),&client_socket_,SLOT(onReconnectTimeout()));
  connect(this,SIGNAL(uploadRecordReady(QString)),&client_socket_,SLOT(onUploadTimeout(QString)));

  connect(&task_timer_,SIGNAL(timeout()),this,SLOT(onTaskTimeout()));
  connect(&task_record_,SIGNAL(triggered(int)),this,SLOT(onTaskRecordTimeout(int)));
  connect(&task_upload_,SIGNAL(triggered(int)),this,SLOT(onTaskUploadTimeout(int)));

  connect(&slave_ble_,SIGNAL(bleRxDataReady()),this,SLOT(onSlaveBleReady()));
  connect(&slave_ble_,SIGNAL(bleDisconnected()),this,SLOT(onSlaveBleDisconnected()));
}
ForkliftClient::~ForkliftClient(){
}

bool ForkliftClient::Begin(ConfigParser::ConfigMap &configs){
  do{//local name and slave_ble
    this->devid_ = "unknown";
    if(configs.contains(CONFIG_KEY_DEVID)){
      devid_ = configs[CONFIG_KEY_DEVID];
    }
    this->slave_mac_ = "";
    if(configs.contains(CONFIG_KEY_SLAVE_BLE)){
      this->slave_mac_ = configs[CONFIG_KEY_SLAVE_BLE];
    }
    QString portname = configs[CONFIG_KEY_BLE_SERIAL];
    if(!slave_ble_.Begin(portname)){
      qDebug()<<tr("[%1,%2]fail to open ble port:%3").arg(__FILE__).arg(__LINE__).arg(portname);
      return false;
    }
    slave_ble_.ConnectToBle(slave_mac_);
    memset((char*)&ble_info_,0,sizeof(ble_info_));
    qDebug()<<tr("[%1,%2]connecting to ble:%3").arg(__FILE__).arg(__LINE__).arg(slave_mac_);
  }while(0);

  //load move state threshold value
  this->move_state_threshold_ = MOVE_STATE_THRESHOLD;
  if(configs.contains(CONFIG_KEY_MOVE_STATE_THRESHOLD)){
    this->move_state_threshold_ = configs[CONFIG_KEY_MOVE_STATE_THRESHOLD].toFloat();
  }
  qDebug()<<tr("[%1,%2]move state threshold:%3m").arg(__FILE__).arg(__LINE__).arg(this->move_state_threshold_);


  do{//local settings
    int task_upload_timeout = SERVER_UPLOAD_TIMEOUT;
    if(configs.contains(CONFIG_KEY_SERVER_UPLOAD_TIMEOUT)){
      task_upload_timeout = configs[CONFIG_KEY_SERVER_UPLOAD_TIMEOUT].toInt();
    }
    task_upload_.Begin(task_upload_timeout);

    this->server_reconnect_timeout_ = SERVER_RECONNECTION_TIMEOUT;
    if(configs.contains(CONFIG_KEY_SERVER_RECONNECT_TIMEOUT)){
      this->server_reconnect_timeout_ = configs[CONFIG_KEY_SERVER_RECONNECT_TIMEOUT].toInt();
    }
    qDebug()<<tr("[%1,%2]upload timeout:%3 ms,server reconnect timeout:%4")
              .arg(__FILE__).arg(__LINE__)
              .arg(task_upload_timeout)
              .arg(server_reconnect_timeout_);
  }while(0);

  do{//open record file
    int task_record_timeout = RECORD_TIMEOUT;
    if(configs.contains(CONFIG_KEY_RECORD_TIMEOUT)){
      task_record_timeout = configs[CONFIG_KEY_RECORD_TIMEOUT].toInt();
    }
    task_record_.Begin(task_record_timeout);

    //step1.upload path
    this->record_upload_path_ = "./record_upload.csv";
    if(configs.contains(CONFIG_KEY_RECORD_UPLOAD_PATH)){
      this->record_upload_path_ = configs[CONFIG_KEY_RECORD_UPLOAD_PATH];
    }

    QString record_tmp_path_="./record_tmp.csv";
    if(configs.contains(CONFIG_KEY_RECORD_TEMP_PATH)){
      record_tmp_path_ = configs[CONFIG_KEY_RECORD_TEMP_PATH];
    }

    //remove tmp file first
    QFile::remove(record_tmp_path_);
    record_tmp_file_.setFileName(record_tmp_path_);
    if(!record_tmp_file_.open(QIODevice::ReadWrite)){
      qDebug()<<tr("[%1,%2]Fail to open the record tmp:%3").arg(__FILE__).arg(__LINE__).arg(record_tmp_path_);
      return false;
    }
    record_tmp_text_.setDevice(&record_tmp_file_);

    qDebug()<<tr("[%1,%2]record path:%3 / %4 at interval:%5")
              .arg(__FILE__).arg(__LINE__)
              .arg(record_tmp_path_)
              .arg(record_upload_path_)
              .arg(task_record_timeout);
  }while(0);


  if(!dist_estimater_.Begin(configs)){
    qDebug()<<tr("[%1,%2]Fail to start dsitance estimater.").arg(__FILE__).arg(__LINE__);
    return false;
  }
  qDebug()<<tr("[%1,%2]distance estimater initialized.").arg(__FILE__).arg(__LINE__);

  if(!client_socket_.Begin(configs)){
    qDebug()<<tr("[%1,%2]Fail to start socket thread.").arg(__FILE__).arg(__LINE__);
    return false;
  }
  qDebug()<<tr("[%1,%2]socket thread initialized.").arg(__FILE__).arg(__LINE__);


  //start server reconnection timer
  reconnect_timer_.start(server_reconnect_timeout_);
  task_timer_.start(TASK_TIMEOUT);

  return true;
}


void ForkliftClient::onTaskTimeout()
{
  int ts = QTime::currentTime().msecsSinceStartOfDay();
  task_record_.Trigger(ts);
  task_upload_.Trigger(ts);
}

void ForkliftClient::onTaskRecordTimeout(int ms)
{
  static float last_distance=0.0f;
  (void)ms;
  int local_time = QTime::currentTime().msecsSinceStartOfDay()/1000;
  int ts = client_socket_.server_time_ +(local_time-client_socket_.local_time_offset_);

  int moving_flag = 0;
  if(fabs(dist_estimater_.Distance()-last_distance)>=this->move_state_threshold_){
    moving_flag=1;
  }
  last_distance = dist_estimater_.Distance();//update last distance
  record_tmp_text_<<ts<<",";
  record_tmp_text_<<devid_<<",";
  record_tmp_text_<<dist_estimater_.Heading()<<",";
  record_tmp_text_<<dist_estimater_.Distance()<<",";
  record_tmp_text_<<dist_estimater_.DistanceAbs()<<",";
  record_tmp_text_<<moving_flag<<",";
  record_tmp_text_<<ble_info_.ir_dist<<",";
  record_tmp_text_<<ble_info_.laser_dist;
  record_tmp_text_<<endl;

  qDebug()<<tr("[%1,%2]record ts:%3,server_ts:%4,local_ts:%5,local_offset:%6")
            .arg(__FILE__).arg(__LINE__)
            .arg(ts)
            .arg(client_socket_.server_time_)
            .arg(local_time)
            .arg(client_socket_.local_time_offset_);
}

void ForkliftClient::onTaskUploadTimeout(int ms)
{
  (void)ms;
  record_tmp_text_.flush();
  if(!record_tmp_file_.copy(this->record_upload_path_)){
    //false means the file exists
    QFile file(record_upload_path_);
    if(!file.open(QIODevice::ReadWrite)){
      QFile::remove(record_upload_path_);
      qDebug()<<tr("[%1,%2]Fail to open the record file:%3").arg(__FILE__).arg(__LINE__).arg(record_upload_path_);
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

  emit uploadRecordReady(this->record_upload_path_);
  //qDebug()<<tr("[%1,%2]called here?").arg(__FILE__).arg(__LINE__);
}


void ForkliftClient::onSlaveBleReady()
{
  ble_info_ = slave_ble_.SlaveInfo();
  client_socket_.volt_slave_ = ble_info_.volt/1000.0f;
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
