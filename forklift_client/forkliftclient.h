#ifndef FORKCLIENT_H
#define FORKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>
#include <QFile>

#include "configparser.h"
#include "serverprotocol.h"
#include "distanceestimater.h"

#define CONFIG_KEY_SERVER_ADDRESS               "server_address"
#define CONFIG_KEY_SERVER_PORT                  "server_port"
#define CONFIG_KEY_SERVER_RECONNECT_INTERVAL    "server_reconnect"

#define CONFIG_KEY_RECORD_TEMP                  "record_temp"
#define CONFIG_KEY_RECORD_UPLOAD                "record_upload"

#define CONFIG_KEY_SLAVE_BLE                    "slave_ble"

class ForkliftClient : public QTcpSocket
{
  Q_OBJECT
public:
  static constexpr int SERVER_RECONNECTION_TIMEOUT=5000;//reconnect every 5 secs

  explicit ForkliftClient(QObject *parent = nullptr);

  bool Begin(ConfigParser::ConfigMap & configs);
  void SetEnable(bool b);
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
  QFile

  QTimer reconnect_timer_;

  DistanceEstimater dist_estimater_;
};

#endif // FORKCLIENT_H
