#include "trajectoryserver.h"
#include <QByteArray>
#include <qdebug.h>

TrajectoryServer::TrajectoryServer(QObject *parent) : QTcpServer(parent)
{
  ptr_client_ = NULL;
  connect(this,SIGNAL(newConnection()),this,SLOT(onNewConnection()));
  connect(&comm_protocol_,SIGNAL(PayloadReady(int,QByteArray&)),this,SLOT(onPayloadReady(int,QByteArray&)));
}

int TrajectoryServer::StartTrackService(const ConfigParser::ConfigMap &configs)
{
  //step1.start service
  int server_port=DEFAULT_SERVER_PORT;
  if(configs.contains(CONFIG_KEY_SERVER_PORT)){
    server_port = configs[CONFIG_KEY_SERVER_PORT].toInt();
  }
  this->listen(QHostAddress::Any,server_port);
  qDebug()<<tr("[%1,%2]Trajectory service started at %3").arg(__FILE__).arg(__LINE__).arg(server_port);

  scale_meter_ = DEFAULT_SCALE_METER;
  if(configs.contains(CONFIG_KEY_SCALE_METER)){
    scale_meter_ = configs[CONFIG_KEY_SCALE_METER].toInt();
  }
  qDebug()<<tr("[%1,%2]Scale %3 pixels/meter").arg(__FILE__).arg(__LINE__).arg(scale_meter_);

  return this->serverPort();
}

void TrajectoryServer::onNewConnection()
{
  QTcpSocket *ptr = this->nextPendingConnection();
  if(ptr_client_!=NULL){
    ptr_client_->disconnectFromHost();
  }
  ptr_client_ = ptr;
  connect(ptr_client_,SIGNAL(readyRead()),this, SLOT(onClientReadyRead()));
  connect(ptr_client_,SIGNAL(disconnected()),this,SLOT(onClientDisconnected()));
  ptr_client_->write("Hey bro, opti server connected.\n");
  emit clientConnected(true);
}

void TrajectoryServer::onClientDisconnected()
{
  this->ptr_client_ = NULL;
  emit clientConnected(false);
}

void TrajectoryServer::onClientReadyRead()
{
  if(this->ptr_client_==NULL)return;
  QByteArray data = ptr_client_->readAll();

  //qDebug()<<tr("[%1,%2]rx pack size:%3").arg(__FILE__).arg(__LINE__).arg(data.size());
  comm_protocol_.PushToProtocol(data);
}

void TrajectoryServer::onPayloadReady(int cmdid, QByteArray &payload)
{
  switch(cmdid){
    case SERVER_UPLOAD_TRACK_REQ:{
      trajectory_point_t *ptr_point = (trajectory_point_t*)payload.data();
      this->yaw_ = ptr_point->heading;
      this->timestamp_ = ptr_point->timestamp;
      this->distance_ = ptr_point->distance;
      this->velocity_ = ptr_point->velocity;
#if 0
      qDebug()<<tr("[%1,%2]ts:%3, yaw:%4, dist:%5,velocity:%6")
                .arg(__FILE__).arg(__LINE__)
                .arg(ptr_point->timestamp)
                .arg(ptr_point->yaw)
                .arg(ptr_point->accumulated_distance)
                .arg(ptr_point->velocity);
#endif
      emit trajectoryUpdated();
      }
      break;
  }
}


