#ifndef FORKCLIENT_H
#define FORKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>

#include "configparser.h"
#include "serverprotocol.h"
#include "distanceestimater.h"

#define CONFIG_KEY_SERVER_ADDRESS       "server_address"
#define CONFIG_KEY_SERVER_PORT          "server_port"
#define CONFIG_KEY_SERVER_RECONNECT_INTERVAL    "server_reconnect"
class ForkClient : public QTcpSocket
{
  Q_OBJECT
public:
  static constexpr int SERVER_RECONNECTION_TIMEOUT=5000;//reconnect every 5 secs

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

  explicit ForkClient(QObject *parent = nullptr);

  bool Begin(ConfigParser::ConfigMap & configs);
public slots:

  void onConnected();
  void onDisconnected();
  void onReadyRead();

  void onReconnectTimeout();

  void onTrajectoryUpdated();
private:
  QString server_address_;
  int server_port_;
  int server_retry_;

  QTimer reconnect_timer_;

  DistanceEstimater dist_estimater_;
};

#endif // FORKCLIENT_H
