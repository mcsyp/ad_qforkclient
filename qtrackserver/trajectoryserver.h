#ifndef TRAJECTORYSERVER_H
#define TRAJECTORYSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>

#include "serverprotocol.h"
#include "configparser.h"

#define CONFIG_KEY_SERVER_PORT "server_port"
#define CONFIG_KEY_SCALE_METER "scale_meter_to_pixel"

class TrajectoryServer : public QTcpServer{
  Q_OBJECT
public:
  static constexpr int DEFAULT_SERVER_PORT=6000;
  static constexpr int DEFAULT_SCALE_METER=150;

  //for server protocol communication:
  //ack server->client
  //req server<-client
  enum SERVER_CMDID {
    SERVER_TIME_REQ=0x1,
    SERVER_TIME_ACK=0x2,
    SERVER_LOGIN_REQ=0x0c,
    SERVER_UPLOAD_TRACK_REQ=0xc0
  };
  //client upload server struct
#pragma pack(1)
  typedef struct trajectory_point_s{
    uint64_t timestamp;
    float heading;//indegree
    float distance;//the accumulated distance in meter
    float distance_abs;
    float delta_x;
    float delta_y;
    float velocity;
  }trajectory_point_t;
#pragma pack()

  explicit TrajectoryServer(QObject *parent = 0);

  int StartTrackService(const ConfigParser::ConfigMap& configs);

  Q_INVOKABLE float yaw() const {return yaw_;}
  Q_INVOKABLE float distance() const {return distance_;}
  Q_INVOKABLE float velocity() const {return velocity_;}

  Q_INVOKABLE int scaleMeterToPixel() const{return scale_meter_;}
signals:
  void trajectoryUpdated();
  void clientConnected(bool connected);
public slots:
  void onNewConnection();

  void onClientDisconnected();
  void onClientReadyRead();
  void onPayloadReady(int cmdid,QByteArray& payload);

protected:
  QTcpSocket* ptr_client_;
  ServeriProtocol comm_protocol_;

  int timestamp_;
  float yaw_;
  float distance_;
  float velocity_;

  int scale_meter_;
};

#endif // TRAJECTORYSERVER_H
