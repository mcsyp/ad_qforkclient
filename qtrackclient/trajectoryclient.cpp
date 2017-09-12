#include "trajectoryclient.h"
#include <qdebug.h>
TrajectoryClient::TrajectoryClient(QObject *parent) : QTcpSocket(parent)
{
  connect(this,SIGNAL(connected()),this,SLOT(onConnected()));
  connect(this,SIGNAL(disconnected()),this,SLOT(onDisconnected()));
  connect(this,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
  connect(&dist_estimater_,SIGNAL(trajectoryUpdated()),this,SLOT(onTrajectoryUpdated()));
  connect(&reconnect_timer_,SIGNAL(timeout()),this,SLOT(onReconnectTimeout()));
}

bool TrajectoryClient::Begin(ConfigParser::ConfigMap &configs){
  do{
    server_address_ = "";
    server_port_=0;
    server_retry_=SERVER_RECONNECTION_TIMEOUT;
    if(configs.contains(CONFIG_KEY_SERVER_ADDRESS)){
      server_address_ = configs[CONFIG_KEY_SERVER_ADDRESS];
    }
    if(configs.contains(CONFIG_KEY_SERVER_PORT)){
      server_port_ = configs[CONFIG_KEY_SERVER_PORT].toInt();
    }
    if(configs.contains(CONFIG_KEY_SERVER_RECONNECT_INTERVAL)){
      server_retry_ = configs[CONFIG_KEY_SERVER_RECONNECT_INTERVAL].toInt();
    }
    if(server_address_.isEmpty() || server_port_==0){
      qDebug()<<tr("[%1,%2]invalid server parameters. quit").arg(__FILE__).arg(__LINE__);
      return false;
    }

    qDebug()<<tr("[%1,%2]connecting to server:%3:%4 at interval %5 ms")
              .arg(__FILE__).arg(__LINE__)
              .arg(server_address_)
              .arg(server_port_)
              .arg(server_retry_);
    //start the reconnection
    connectToHost(server_address_,server_port_);
  }while(0);

  if(!dist_estimater_.Begin(configs)){
    qDebug()<<tr("[%1,%2]Fail to begin the esimator").arg(__FILE__).arg(__LINE__);
    return false;
  }
  //start server reconnection timer
  reconnect_timer_.start(server_retry_);

  qDebug()<<tr("[%1,%2]pack size:%5").arg(__FILE__).arg(__LINE__).arg(sizeof(trajectory_point_t));

  return true;
}
void TrajectoryClient::onReconnectTimeout(){
  qDebug()<<tr("[%1,%2]connecting to server:%3:%4 at interval %5 ms")
            .arg(__FILE__).arg(__LINE__)
            .arg(server_address_)
            .arg(server_port_)
            .arg(server_retry_);
  //start the reconnection
  connectToHost(server_address_,server_port_);
}

void TrajectoryClient::onTrajectoryUpdated()
{
  uint8_t head[32];

  trajectory_point_t p;
  p.timestamp = dist_estimater_.LastUpdateTimestamp();
  p.heading = dist_estimater_.Heading();
  p.distance = dist_estimater_.Distance();
  dist_estimater_.DistanceDelta(&(p.delta_x),&(p.delta_y));
  p.distance_abs = dist_estimater_.DistanceAbs();
  p.velocity = dist_estimater_.Velocity();
#if 0
  qDebug()<<tr("[%1,%2]ts:%3, yaw:%4, dist:%5")
            .arg(__FILE__).arg(__LINE__)
            .arg(p.timestamp)
            .arg(p.yaw)
            .arg(p.accumulated_distance);
#endif
  int message_size = ServeriProtocol::FillHead(SERVER_UPLOAD_TRACK_REQ,sizeof(p),head,32);
  write((char*)head,message_size);
  write((char*)(&p),sizeof(p));
}

void TrajectoryClient::onConnected()
{
  qDebug()<<tr("[%1,%2]server connected").arg(__FILE__).arg(__LINE__);
  reconnect_timer_.stop();
}

void TrajectoryClient::onDisconnected()
{
  qDebug()<<tr("[%1,%2]server disconnected").arg(__FILE__).arg(__LINE__);
  reconnect_timer_.start(server_retry_);
}

void TrajectoryClient::onReadyRead()
{
  QByteArray data = readAll();
  qDebug()<<tr("[%1,%2]rx message:%3").arg(__FILE__).arg(__LINE__).arg(QString(data));
}
